#include <lib.h>
#include <stdio.h>
#include <minix/rs.h>
#include <minix/sysutil.h>
#include <minix/callnr.h>
#include <minix/endpoint.h>
#include <sys/types.h>

// Function prototypes
int do_add(int a, int b);
int do_multiply(int a, int b);

int main(void) {
    message m;
    int result;

    printf("Calc server started.\n");

    while (TRUE) {
        if (minix_receive(ANY, &m) != OK) {
            printf("minix_receive error.\n");
            continue;
        }

        switch (m.m_type) {
            case CALC_ADD:
                result = do_add(m.m1_i1, m.m1_i2);
                m.m_type = result;
                minix_send(m.m_source, &m);
                break;

            case CALC_MULTIPLY:
                result = do_multiply(m.m1_i1, m.m1_i2);
                m.m_type = result;
                minix_send(m.m_source, &m);
                break;

            default:
                printf("Unknown message type: %d\n", m.m_type);
                break;
        }
    }
    return 0;
}

// Function definitions
int do_add(int a, int b) {
    return a + b;
}

int do_multiply(int a, int b) {
    message m;
    m.m1_i1 = a;
    m.m1_i2 = b;

    // This is the kernel call to perform multiplication
    int result = _kernel_call(SYS_MULTIPLY, &m);

    if (result != OK) {
        printf("Kernel call failed: %d\n", result);
        return result;
    }

    return m.m1_i3; // The result is returned in m1_i3
}
