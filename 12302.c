clearop(oparg_T *oap)
{
    oap->op_type = OP_NOP;
    oap->regname = 0;
    oap->motion_force = NUL;
    oap->use_reg_one = FALSE;
    motion_force = NUL;
}