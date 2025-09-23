#include "syslib.h"

int sys_sample(unsigned flags, endpoint_t proc_ep)
{
    message m;
    
    /* Prepare the message */
    m.m_lsys_krn_sample.flags = flags;
    m.m_lsys_krn_sample.endpoint = proc_ep;
    
    return(_kernel_call(SYS_SAMPLE, &m));
}
