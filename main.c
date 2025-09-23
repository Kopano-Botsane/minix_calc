#include "kernel/system.h"
#include <minix/endpoint.h>
#include <minix/sys_config.h>

/*===========================================================================*
 *                                do_sample                                  *
 *===========================================================================*/
int do_sample(struct proc *caller_ptr, message *m_ptr)
{
    endpoint_t target_ep;
    struct proc *target_proc;
    
    /* Extract parameters from the message */
    target_ep = m_ptr->m_lsys_krn_sample.endpoint;
    
    /* Find the target process */
    if(!isokendpt(target_ep, &target_ep)) {
        return EINVAL;  /* Invalid endpoint */
    }
    
    target_proc = proc_addr(target_ep);
    
    /* Do something useful - example: return process priority */
    m_ptr->m_lsys_krn_sample.result = target_proc->p_priority;
    
    printf("do_sample: Called for process %d, priority: %d\n", 
           target_ep, target_proc->p_priority);
    
    return OK;
}
