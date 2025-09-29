#include <minix/endpoint.h>
#include <minix/rs.h>
#include <minix/sysutil.h>
#include "calc.h"
#include "mtype.h"
#include "proto.h"

/* Global input message */
message m_in;

/* External dependencies */
extern int map_size;
extern int (* const call_vec[])(message *m_ptr);

/*
 *===========================================================================
 * main
 *===========================================================================
 * The main routine of the Calculator server.
 */
int main(void)
{
  endpoint_t who_e;
  int ipc_status;
  int r;

  /* Register with Reincarnation Server (RS) */
  if (rs_init(RS_CALC) != OK) {
      panic("CALC: rs_init failed");
  }

  /* Main message loop */
  while (TRUE) {
    /* Wait for a request message */
    r = receive(ANY, &m_in, &ipc_status);
    
    if (r != OK) {
      printf("CALC: receive failed: %d\n", r);
      continue;
    }

    who_e = m_in.m_source;
    
    if (is_ipc_notify(ipc_status)) {
        /* Handle notifications (not relevant for this simple task) */
        continue;
    }

    /* Message type is CALC_ADD or CALC_MULTIPLY. 
     * Since CALC_BASE is offset in callnr.h, we map it back to 0 or 1.
     */
    int call_num = m_in.m_type - CALC_BASE;

    if (call_num < 0 || call_num >= map_size) {
        printf("CALC: illegal system call %d from %d\n", m_in.m_type, who_e);
        r = EINVAL; /* Invalid argument */
    } else {
        /* Dispatch the call to the appropriate handler function */
        r = call_vec[call_num](&m_in);
    }
    
    /* Send the reply back to the user process */
    m_in.m_type = r; /* Use return code as reply type */
    sendnb(who_e, &m_in);
  }
}
