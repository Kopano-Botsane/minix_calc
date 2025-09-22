/* /usr/src/servers/calc/calc.c */
#include <sys/cdefs.h>
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <minix/rs.h>
#include <minix/syslib.h>
#include "calc.h"

/* Forward declarations for message handlers */
static int do_add(message *m_ptr);
static int do_multiply(message *m_ptr);

/* Main server loop */
int main(void)
{
    message m;
    int r, ipc_status;
    
    printf("Calc server: starting\n");
    
    /* Main message loop */
    while (TRUE) {
        /* Wait for incoming message */
        if ((r = sef_receive(ANY, &m, &ipc_status)) != OK) {
            printf("Calc: sef_receive failed: %d\n", r);
            continue;
        }
        
        /* Handle message based on type */
        switch(m.m_type) {
            case CALC_ADD:
                m.m_type = do_add(&m);
                break;
                
            case CALC_MULTIPLY:
                m.m_type = do_multiply(&m);
                break;
                
            default:
                printf("Calc: unknown message type: %d\n", m.m_type);
                m.m_type = EINVAL;
                break;
        }
        
        /* Send reply */
        if ((r = ipc_send(m.m_source, &m)) != OK) {
            printf("Calc: ipc_send failed: %d\n", r);
        }
    }
    
    return 0;
}

/* Add service implementation */
static int do_add(message *m_ptr)
{
    calc_add_t *req = (calc_add_t *)m_ptr;
    int result = req->a + req->b;
    
    printf("Calc: add(%d, %d) = %d\n", req->a, req->b, result);
    
    /* Store result in message */
    m_ptr->m_calc_add_response.result = result;
    return OK;
}

/* Multiply service implementation */
static int do_multiply(message *m_ptr)
{
    calc_multiply_t *req = (calc_multiply_t *)m_ptr;
    int result;
    
    /* Call kernel multiplication service */
    if (sys_multiply(req->a, req->b, &result) != OK) {
        printf("Calc: kernel multiply failed\n");
        return EIO;
    }
    
    printf("Calc: multiply(%d, %d) = %d\n", req->a, req->b, result);
    
    /* Store result in message */
    m_ptr->m_calc_multiply_response.result = result;
    return OK;
}