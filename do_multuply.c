int do_multiply(struct proc *caller_ptr, message *m_ptr)
{
    m_ptr->m1_i1 = m_ptr->m1_i1 * m_ptr->m1_i2;
    return OK;
}
