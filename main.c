#include <lib.h>
#include <stdio.h>
#include <minix/rs.h>
#include <minix/sysutil.h>
#include <minix/callnr.h>
#include <minix/endpoint.h>
#include <sys/types.h>
#include "proto.h"

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
