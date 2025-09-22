/* 
 * calc - Calculator Server
 * 
 * A simple system process that handles addition requests.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <minix/endpoint.h>
#include <minix/syslib.h>

#include "calc.h"

/* Function prototype */
static void handle_add_request(calc_message_t *msg, endpoint_t src);

/* Main function of the calc server */
int main(int argc, char **argv)
{
    int r;
    endpoint_t src;
    calc_message_t msg;
    
    printf("Calc server: starting up\n");
    
    /* Main message loop */
    while (1) {
        /* Wait for any incoming message */
        r = sef_receive(ANY, &msg, &src);
        
        if (r != OK) {
            printf("Calc server: sef_receive failed with %d\n", r);
            continue;
        }
        
        /* Handle message based on type */
        switch (msg.m_type) {
            case CALC_ADD_REQUEST:
                handle_add_request(&msg, src);
                break;
                
            default:
                /* Unknown message type, send error reply */
                msg.m_type = CALC_ADD_REPLY;
                msg.status = EINVAL;  /* Invalid argument */
                r = send(src, &msg);
                if (r != OK) {
                    printf("Calc server: failed to send error reply to %d\n", src);
                }
                break;
        }
    }
    
    return 0;  /* Should never reach here */
}

/* Handle an addition request */
static void handle_add_request(calc_message_t *msg, endpoint_t src)
{
    int r;
    
    printf("Calc server: received add request from %d: %d + %d\n", 
           src, msg->operand1, msg->operand2);
    
    /* Perform the addition */
    msg->result = msg->operand1 + msg->operand2;
    msg->status = 0;  /* Success */
    msg->m_type = CALC_ADD_REPLY;
    
    /* Send the reply back to the sender */
    r = send(src, msg);
    
    if (r != OK) {
        printf("Calc server: failed to send reply to %d: %d\n", src, r);
    } else {
        printf("Calc server: sent reply to %d: result = %d\n", src, msg->result);
    }
}
