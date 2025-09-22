/* This task handles the interface between the kernel and user-level servers.
 * System services can be accessed by doing a system call. System calls are
 * transformed into request messages, which are handled by this task. By
 * convention, a sys_call() is transformed in a SYS_CALL request message that
 * is handled in a function named do_call().
 *
 * A private call vector is used to map all system calls to the functions that
 * handle them. The actual handler functions are contained in separate files
 * to keep this file clean. The call vector is used in the system task's main
 * loop to handle all incoming requests.
 *
 * In addition to the main sys_task() entry point, which starts the main loop,
 * there are several other minor entry points:
 *   get_priv:		assign privilege structure to user or system process
 *   set_sendto_bit:	allow a process to send messages to a new target
 *   unset_sendto_bit:	disallow a process from sending messages to a target
 *   fill_sendto_mask:	fill the target mask of a given process
 *   send_sig:		send a signal directly to a system process
 *   cause_sig:		take action to cause a signal to occur via a signal mgr
 *   sig_delay_done:	tell PM that a process is not sending
 *   send_diag_sig:	send a diagnostics signal to interested processes
 *   get_randomness:	accumulate randomness in a buffer
 *   clear_endpoint:	remove a process' ability to send and receive messages
 *   sched_proc:	schedule a process
 *
 * Changes:
*    Nov 22, 2009   get_priv supports static priv ids (Cristiano Giuffrida)
 *   Aug 04, 2005   check if system call is allowed  (Jorrit N. Herder)
 *   Jul 20, 2005   send signal to services with message  (Jorrit N. Herder)
 *   Jan 15, 2005   new, generalized virtual copy function  (Jorrit N. Herder)
 *   Oct 10, 2004   dispatch system calls from call vector  (Jorrit N. Herder)
 *   Sep 30, 2004   source code documentation updated  (Jorrit N. Herder)
 */

#include "kernel/system.h"
#include "kernel/vm.h"
#include "kernel/clock.h"
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <minix/endpoint.h>
#include <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
static int (*call_vec[NR_SYS_CALLS])(struct proc * caller, message *m_ptr);

#define map(call_nr, handler) 					\
    {	int call_index = call_nr-KERNEL_CALL; 				\
    	assert(call_index >= 0 && call_index < NR_SYS_CALLS);			\
    call_vec[call_index] = (handler)  ; }

static void kernel_call_finish(struct proc * caller, message *msg, int result)
{
  if(result == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  assert(RTS_ISSET(caller, RTS_VMREQUEST));
	  assert(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *msg;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } else {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_source = NONE;
	  if (result != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  msg->m_source = SYSTEM;
		  msg->m_type = result;		/* report status of call */
#if DEBUG_IPC_HOOK
	hook_ipc_msgkresult(msg, caller);
#endif
		  if (copy_msg_to_user(msg, (message *)caller->p_delivermsg_vir)) {
			  printf("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

static int kernel_call_dispatch(struct proc * caller, message *msg)
{
  int result = OK;
  int call_nr;

#if DEBUG_IPC_HOOK
	hook_ipc_msgkcall(msg, caller);
#endif
  call_nr = msg->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  if (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  printf("SYSTEM: illegal request %d from %d.\n",
			  call_nr,msg->m_source);
	  result = EBADREQUEST;			/* illegal message type */
  }
  else if (!GET_BIT(priv(caller)->s_k_call_mask, call_nr)) {
	  printf("SYSTEM: denied request %d from %d.\n",
			  call_nr,msg->m_source);
	  result = ECALLDENIED;			/* illegal message type */
  } else {
	  /* handle the system call */
	  if (call_vec[call_nr])
		  result = (*call_vec[call_nr])(caller, msg);
	  else {
		  printf("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  result = EBADREQUEST;
	  }
  }

  return result;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
void kernel_call(message *m_user, struct proc * caller)
{
  int result = OK;
  message msg;

  caller->p_delivermsg_vir = (vir_bytes) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  if (copy_msg_from_user(m_user, &msg) == 0) {
	  msg.m_source = caller->p_endpoint;
	  result = kernel_call_dispatch(caller, &msg);
  }
  else {
	  printf("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  return;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_finish(caller, &msg, result);
}

/*===========================================================================*
 *				do_add					     *
 *===========================================================================*/
int do_add(struct proc *caller, message *m_ptr)
{
/* Handle the ADD system call. This call forwards an addition request to the
 * calc server and returns the result to the caller.
 */
    int a, b, result;
    endpoint_t calc_ep;
    int r;
    message calc_msg;

    /* Get the operands from the message */
    a = m_ptr->m1_i1;  /* First operand */
    b = m_ptr->m1_i2;  /* Second operand */

    /* Look up the calc server endpoint */
    r = minix_rs_lookup("calc", &calc_ep);
    if (r != OK) {
        printf("SYSTEM: calc server not found\n");
        return r;
    }

    /* Prepare message for calc server */
    calc_msg.m_type = CALC_ADD_REQUEST;
    calc_msg.m1_i1 = a;
    calc_msg.m1_i2 = b;

    /* Send the request to calc server and wait for reply */
    r = ipc_sendrec(calc_ep, &calc_msg);
    if (r != OK) {
        printf("SYSTEM: ipc_sendrec to calc failed: %d\n", r);
        return r;
    }

    /* Check if we got a proper reply */
    if (calc_msg.m_type != CALC_ADD_REPLY) {
        printf("SYSTEM: unexpected reply type from calc: %d\n", calc_msg.m_type);
        return EIO;
    }

    /* Get the result from the calc server's reply */
    result = calc_msg.m1_i1;

    /* Return the result to the user */
    m_ptr->m1_i1 = result;  /* Put result in reply message */
    
    return OK;
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
void system_init(void)
{
  register struct priv *sp;
  int i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  for (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  for (sp=BEG_PRIV_ADDR; sp < END_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  for (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = NULL;
  }

  /* Process management. */
  map(SYS_FORK, do_fork); 		/* a process forked a new process */
  map(SYS_EXEC, do_exec);		/* update process after execute */
  map(SYS_CLEAR, do_clear);		/* clean up after process exit */
  map(SYS_EXIT, do_exit);		/* a system process wants to exit */
  map(SYS_PRIVCTL, do_privctl);		/* system privileges control */
  map(SYS_TRACE, do_trace);		/* request a trace operation */
  map(SYS_SETGRANT, do_setgrant);	/* get/set own parameters */
  map(SYS_RUNCTL, do_runctl);		/* set/clear stop flag of a process */
  map(SYS_UPDATE, do_update);		/* update a process into another */
  map(SYS_STATECTL, do_statectl);	/* let a process control its state */

  /* Signal handling. */
  map(SYS_KILL, do_kill); 		/* cause a process to be signaled */
  map(SYS_GETKSIG, do_getksig);		/* signal manager checks for signals */
  map(SYS_ENDKSIG, do_endksig);		/* signal manager finished signal */
  map(SYS_SIGSEND, do_sigsend);		/* start POSIX-style signal */
  map(SYS_SIGRETURN, do_sigreturn);	/* return from POSIX-style signal */

  /* Device I/O. */
  map(SYS_IRQCTL, do_irqctl);  		/* interrupt control operations */
#if defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#endif

  /* Memory management. */
  map(SYS_MEMSET, do_memset);		/* write char to memory area */
  map(SYS_VMCTL, do_vmctl);		/* various VM process settings */

  /* Copying. */
  map(SYS_UMAP, do_umap);		/* map virtual to physical address */
  map(SYS_UMAP_REMOTE, do_umap_remote);	/* do_umap for non-caller process */
  map(SYS_VUMAP, do_vumap);		/* vectored virtual to physical map */
  map(SYS_VIRCOPY, do_vircopy); 	/* use pure virtual addressing */
  map(SYS_PHYSCOPY, do_copy);	 	/* use physical addressing */
  map(SYS_SAFECOPYFROM, do_safecopy_from);/* copy with pre-granted permission */
  map(SYS_SAFECOPYTO, do_safecopy_to);	/* copy with pre-granted permission */
  map(SYS_VSAFECOPY, do_vsafecopy);	/* vectored safecopy */

  /* safe memset */
  map(SYS_SAFEMEMSET, do_safememset);	/* safememset */

  /* Clock functionality. */
  map(SYS_TIMES, do_times);		/* get uptime and process times */
  map(SYS_SETALARM, do_setalarm);	/* schedule a synchronous alarm */
  map(SYS_STIME, do_stime);		/* set the boottime */
  map(SYS_SETTIME, do_settime);		/* set the system time (realtime) */
  map(SYS_VTIMER, do_vtimer);		/* set or retrieve a virtual timer */

  /* System control. */
  map(SYS_ABORT, do_abort);		/* abort MINIX */
  map(SYS_GETINFO, do_getinfo); 	/* request system information */
  map(SYS_DIAGCTL, do_diagctl);		/* diagnostics-related functionality */

  /* Profiling. */
  map(SYS_SPROF, do_sprofile);         /* start/stop statistical profiling */

  /* arm-specific. */
#if defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#endif

  /* i386-specific. */
#if defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#endif

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_SCHEDULE, do_schedule);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

  /* Calculator system calls */
  map(ADD, do_add);			/* perform addition via calc server */

}

/* ... rest of the file remains exactly the same ... */

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
int priv_add_mem(struct proc *rp, struct minix_mem_range *memr)
{
        struct priv *priv = priv(rp);
        int i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	for (i = 0; i < priv->s_nr_mem_range; i++) {
		if (priv->s_mem_tab[i].mr_base == memr->mr_base &&
			priv->s_mem_tab[i].mr_limit == memr->mr_limit)
			return OK;
	}

	i= priv->s_nr_mem_range;
	if (i >= NR_MEM_RANGE) {
		printf("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		return ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	return OK;
}
