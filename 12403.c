checkclearopq(oparg_T *oap)
{
    if (oap->op_type == OP_NOP && !VIsual_active)
	return FALSE;
    clearopbeep(oap);
    return TRUE;
}