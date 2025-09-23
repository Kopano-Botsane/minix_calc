#include <minix/ipc.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_REQUEST 1

int main(void)
{
    message m;
    endpoint_t sample_ep;

    // Look up the endpoint of the "sample" server
    if (ds_retrieve_label_endpt("sample", &sample_ep) != OK) {
        fprintf(stderr, "Cannot find sample server\n");
        return 1;
    }

    m.m_type = SAMPLE_REQUEST;

    if (sendrec(sample_ep, &m) != OK) {
        fprintf(stderr, "Failed to communicate with sample server\n");
        return 1;
    }

    printf("Reply from sample server: %d\n", m.m_u.m_m1.m1i1);
    return 0;
}
