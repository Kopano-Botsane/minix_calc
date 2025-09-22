/* /usr/src/servers/calc/calc.h */
#ifndef _CALC_H
#define _CALC_H

/* Message types */
#define CALC_ADD          0x5000
#define CALC_MULTIPLY     0x5001

/* Message structures */
typedef struct {
    int a;
    int b;
} calc_add_t;

typedef struct {
    int a;
    int b;
} calc_multiply_t;

typedef struct {
    int result;
} calc_response_t;

/* Union for message handling */
typedef struct {
    int m_type;
    union {
        calc_add_t      m_calc_add;
        calc_multiply_t m_calc_multiply;
        calc_response_t m_calc_add_response;
        calc_response_t m_calc_multiply_response;
    } m_u;
} message;

#define m_calc_add           m_u.m_calc_add
#define m_calc_multiply      m_u.m_calc_multiply
#define m_calc_add_response  m_u.m_calc_add_response
#define m_calc_multiply_response m_u.m_calc_multiply_response

#endif /* _CALC_H */