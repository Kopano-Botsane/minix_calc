/* calc.h - Message structure for the calculator server */

#ifndef _CALC_H_
#define _CALC_H_

/* Message type for addition request */
#define CALC_ADD_REQUEST   1
#define CALC_ADD_REPLY     2

/* Message structure for addition */
typedef struct {
    int m_type;           /* Message type: CALC_ADD_REQUEST or CALC_ADD_REPLY */
    int operand1;         /* First operand */
    int operand2;         /* Second operand */
    int result;           /* Result of the operation */
    int status;           /* Status of the operation (0 for success, error code otherwise) */
} calc_message_t;

#endif /* _CALC_H_ */
