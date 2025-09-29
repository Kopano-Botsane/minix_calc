#include <minix/syslib.h>
#include <minix/callnr.h>
#include "calc.h"
#include "mtype.h"
#include "proto.h"

/* The global server message pointer (input message) */
extern message m_in;

/*
 *===========================================================================
 * do_add
 *===========================================================================
 * Handles CALC_ADD: simple user-space addition.
 * The result is placed directly into the reply message.
 */
int do_add(message *m_ptr)
{
  int a = m_ptr->m1_i1;
  int b = m_ptr->m1_i2;
  int result = a + b;
  
  m_ptr->m1_i1 = result; /* Place result in the reply message */
  
  return(OK);
}

/*
 *===========================================================================
 * do_multiply
 *===========================================================================
 * Handles CALC_MULTIPLY: server calls the kernel to perform the operation.
 */
int do_multiply(message *m_ptr)
{
  int status;
  int result;

  /*
   * Call the kernel function sys_multiply, passing the arguments 
   * extracted from the user's message and a pointer for the result.
   */
  status = sys_multiply(m_ptr->m1_i1, m_ptr->m1_i2, &result);

  if (status != OK) {
      return status;
  }
  
  m_ptr->m1_i1 = result; /* Place result from kernel back into reply message */

  return(OK);
}
