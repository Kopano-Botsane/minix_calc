/* Calculator Service.
 * This service handles addition requests from user processes.
 *
 * Created based on the Information Server (is) template.
 */

#include "inc.h"
#include <minix/endpoint.h>

/* Allocate space for the global variables. */
static message m_in;		/* the input message itself */
static message m_out;		/* the output message used for reply */
static endpoint_t who_e;	/* caller's proc number */
static int callnr;		/* system call number */

/* Declare some local functions. */
static void get_work(void);
static void reply(int whom, int result);
static int do_add(message *m_ptr);

/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char **argv)
{
  int result;

  /* SEF local startup. */
  env_setargs(argc, argv);
  sef_local_startup();

  /* Main loop - get work and do it, forever. */
  while (TRUE) {
      /* Wait for incoming message, sets 'callnr' and 'who'. */
      get_work();

      /* Only handle CALC_ADD messages for now */
      if (callnr == CALC_ADD) {
          result = do_add(&m_in);
      } else {
          printf("CALC: warning, got illegal request %d from %d\n",
              callnr, m_in.m_source);
          result = EDONTREPLY;
      }

      /* Finally send reply message, unless disabled. */
      if (result != EDONTREPLY) {
          reply(who_e, result);
      }
  }
  return(OK);
}

/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
static void sef_local_startup(void)
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_lu(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
static int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *UNUSED(info))
{
  /* Initialize the calculator server. */
  return(OK);
}

/*===========================================================================*
 *				do_add					     *
 *===========================================================================*/
static int do_add(message *m_ptr)
{
    /* Perform the addition operation */
    int a = m_ptr->m_calc_lsys_add.num1;
    int b = m_ptr->m_calc_lsys_add.num2;
    int sum = a + b;
    
    /* Store result in reply message */
    m_out.m_calc_lsys_add.result = sum;
    
    return(OK);
}

/*===========================================================================*
 *				get_work                                     *
 *===========================================================================*/
static void get_work(void)
{
    int status = 0;
    status = sef_receive(ANY, &m_in);   /* blocks until message arrives */
    if (OK != status)
        panic("sef_receive failed!: %d", status);
    who_e = m_in.m_source;        /* message arrived! set sender */
    callnr = m_in.m_type;       /* set function call number */
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(int who, int result)
{
    int send_status;
    m_out.m_type = result;  		/* build reply message */
    send_status = ipc_send(who, &m_out);    /* send the message */
    if (OK != send_status)
        panic("unable to send reply!: %d", send_status);
}
