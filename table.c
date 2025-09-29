#include <minix/callnr.h>
#include <minix/config.h>
#include "calc.h"
#include "mtype.h"
#include "proto.h"

/*
 * Message types for the CALC server are simple integers (0, 1, ...).
 * The order here must match the DO_ constants in mtype.h.
 */
int (* const call_vec[])(message *m_ptr) = {
/* 00 */    do_add,        /* DO_ADD_MSG (CALC_ADD) */
/* 01 */    do_multiply,   /* DO_MULTIPLY_MSG (CALC_MULTIPLY) */
};

/* The total number of system calls handled by the CALC server */
int map_size = sizeof(call_vec) / sizeof(call_vec[0]);
