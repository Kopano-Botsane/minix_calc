PUBLIC int calc_add(int a, int b)
{
    message m;
    m.m2_i1 = a;
    m.m2_i2 = b;
    m.m2_i3 = CALC_ADD;
    return _syscall(PM_PROC_NR, PM_CALC, &m);
}

PUBLIC int calc_multiply(int a, int b)
{
    message m;
    m.m2_i1 = a;
    m.m2_i2 = b;
    m.m2_i3 = CALC_MULTIPLY;
    return _syscall(PM_PROC_NR, PM_CALC, &m);
}
