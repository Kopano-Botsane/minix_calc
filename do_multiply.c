#include <minix/syslib.h>
#include <minix/type.h>
#include "system.h" /* For access to system message structures */

/*
 *===========================================================================
 * do_multiply
 *===========================================================================
 * This function handles the MINIX_KERN_MULTIPLY kernel call from a server.
 * It multiplies the two integers provided by the server and returns the result.
 */
int do_multiply(message *m_ptr)
{
  int a, b;
  long result; /* Use long to safely check for overflow */

  /* The message from the CALC server will contain the arguments (m_lc_v1)
   * which were originally sent to the server by the user process (m1_i1, m1_i2).
   */
  a = m_ptr->m_lc_v1.m1_i1;
  b = m_ptr->m_lc_v1.m1_i2;

  /* Perform calculation. We use 'long' to check for simple integer overflow,
   * though in a real system, more rigorous checks would be needed.
   */
  result = (long)a * (long)b;

  /* Put the result back into the message structure to be returned to the
   * CALC server which initiated this kernel call.
   */
  m_ptr->m_lc_v1.m1_i1 = (int)result;

  return(OK);
}
