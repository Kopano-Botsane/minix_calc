#include "inc.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/endpoint.h>
#include <minix/ipc.h>
#include <stdio.h>
#include <stdlib.h>

/* Function prototypes. */
static void sef_local_startup(void);
static void sef_local_message_process(message *m, int call_type);
int do_add(message *m);
int do_multiply(message *m);

/*===========================================================================*
 * main                                           *
 *===========================================================================*/
int main(void) {
    message m;
    int ipc_status;
    int call_nr, result;

    /* SEF local startup. */
    sef_local_startup();

    /* Main loop - get work and do it, forever. */
    while (TRUE) {
        /* Wait for request message. */
        if (sef_receive_status(ANY, &m, &ipc_status) != OK) {
            continue;
        }

        /* Handle the request and send a reply to the caller. */
        call_nr = m.m_type;
        
        switch (call_nr) {
            case CALC_ADD:
                result = do_add(&m);
                break;
            case CALC_MULTIPLY:
                result = do_multiply(&m);
                break;
            default:
                printf("CALC: warning: got unexpected request %d from %d\n",
                       m.m_type, m.m_source);
                result = ENOSYS;
                break;
        }
        
        if (result != EDONTREPLY) {
            m.m_type = result;
            reply(m.m_source, NULL, &m);
        }
    }

    return 0;
}

---

/*===========================================================================*
 * sef_local_startup                                  *
 *===========================================================================*/
static void sef_local_startup() {
    /* Register init callbacks. */
    sef_setcb_init_fresh(sef_cb_init_fresh);
    sef_setcb_init_restart(sef_cb_init_restart);
    sef_setcb_init_lu(sef_cb_init_lu);

    /* Let SEF perform startup. */
    sef_startup();
}

/* Add the sef_cb_init_* functions and other supporting code as needed. */

/*===========================================================================*
 * do_add                                             *
 *===========================================================================*/
int do_add(message *m) {
    /* Implementation for the add service will go here. */
    return OK;
}

/*===========================================================================*
 * do_multiply                                        *
 *===========================================================================*/
int do_multiply(message *m) {
    /* Implementation for the multiply service will go here. */
    return OK;
}
