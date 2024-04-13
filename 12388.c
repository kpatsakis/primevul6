checkclearop(oparg_T *oap)
{
    if (oap->op_type == OP_NOP)
	return FALSE;
    clearopbeep(oap);
    return TRUE;
}