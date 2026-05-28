/* Calc Server */

#include "inc.h"
#include <minix/endpoint.h>

/* Allocate space for the global variables. */
PRIVATE endpoint_t who_e;       /* caller's proc number */
PRIVATE int callnr;             /* system call number */

/* Declare some local functions. */
FORWARD _PROTOTYPE(void get_work, (message *m_ptr));
FORWARD _PROTOTYPE(void reply, (endpoint_t whom, message *m_ptr));

/* SEF functions and variables. */
FORWARD _PROTOTYPE( void sef_local_startup, (void) );

PUBLIC int main(int argc, char **argv)
{
    message m;
    int result;

    env_setargs(argc, argv);
    sef_local_startup();

    while (TRUE) {
        get_work(&m);

        if (is_notify(callnr)) {
            result = EINVAL;
            goto send_reply;
        }

        switch (callnr) {
        case CALC_ADD:
            result = (int)m.m2_l1 + (int)m.m2_l2;
            m.m2_i1 = result;
            break;

        case CALC_MULTIPLY:
            m.m_type = SYS_MULTIPLY;
            m.m1_i1 = (int)m.m2_l1;
            m.m1_i2 = (int)m.m2_l2;
            result = _kernel_call(SYS_MULTIPLY, &m);
            break;

        default:
            result = EINVAL;
        }

send_reply:
        if (result != EDONTREPLY) {
            m.m_type = result;
            reply(who_e, &m);
        }
    }
    return OK;
}
