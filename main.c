#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sef.h>
#include <stdio.h>

#define SAMPLE_REQUEST 1

int main(void)
{
    message m;
    endpoint_t src;

    printf("Sample server started (user space)\n");

    // Register with Reincarnation Server (RS)
    sef_startup();

    while (1) {
        int r = sef_receive(ANY, &m);
        if (r != OK) continue;

        src = m.m_source;

        if (m.m_type == SAMPLE_REQUEST) {
            printf("Received SAMPLE request from endpoint %d\n", src);
            m.m_type = 0;              // reply type
            m.m_u.m_m1.m1i1 = 1234;    // send back some value
            send(src, &m);
        }
    }
}
