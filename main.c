#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/rs.h>
#include <minix/callnr.h>
#include <minix/calc.h>
#include <minix/com.h>
#include <minix/endpoint.h>
#include <minix/safecopies.h>
#include <minix/sys_config.h>
#include <minix/vm.h>
#include <minix/timers.h>
#include <minix/sched.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

/* Function prototypes */
static void calc_init(void);
static void calc_work(void);
static int do_add(message *m_ptr);
static int do_multiply(message *m_ptr);
static int kernel_multiply(int a, int b);

/* Main function */
int main(int argc, char **argv)
{
    calc_init();
    calc_work();
    return 0;
}

/* Initialize the calc server */
static void calc_init(void)
{
    /* Register with the system */
    int r;
    if ((r = sys_register("calc")) != OK) {
        panic("calc: sys_register failed: %d\n", r);
    }
    printf("Calc server started\n");
}

/* Main work loop */
static void calc_work(void)
{
    message m;
    int r;
    endpoint_t src;
    
    while (TRUE) {
        /* Wait for incoming messages */
        if ((r = sef_receive(ANY, &m)) != OK) {
            printf("calc: sef_receive failed: %d\n", r);
            continue;
        }
        
        src = m.m_source;
        
        /* Handle the message based on type */
        switch (m.m_type) {
            case CALC_ADD:
                m.m_type = do_add(&m);
                break;
                
            case CALC_MULTIPLY:
                m.m_type = do_multiply(&m);
                break;
                
            default:
                m.m_type = EINVAL;
                break;
        }
        
        /* Send reply back to caller */
        if ((r = send(src, &m)) != OK) {
            printf("calc: send to %d failed: %d\n", src, r);
        }
    }
}

/* Handle add operation */
static int do_add(message *m_ptr)
{
    calc_message_t *calc_msg = (calc_message_t *)m_ptr;
    calc_msg->result = calc_msg->num1 + calc_msg->num2;
    return OK;
}

/* Handle multiply operation */
static int do_multiply(message *m_ptr)
{
    calc_message_t *calc_msg = (calc_message_t *)m_ptr;
    
    /* Call kernel function for multiplication */
    calc_msg->result = kernel_multiply(calc_msg->num1, calc_msg->num2);
    
    if (calc_msg->result < 0) {
        return EINVAL;  /* Error in kernel call */
    }
    
    return OK;
}

/* Kernel call wrapper for multiplication */
static int kernel_multiply(int a, int b)
{
    message m;
    int result;
    
    /* Prepare message for kernel call */
    m.m_calc_lsys_multiply.num1 = a;
    m.m_calc_lsys_multiply.num2 = b;
    
    /* Make kernel call */
    if (_taskcall(SYSTASK, SYS_CALC_MULTIPLY, &m) != OK) {
        return -1;
    }
    
    return m.m_calc_lsys_multiply.result;
}
