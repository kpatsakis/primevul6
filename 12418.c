nv_Undo(cmdarg_T *cap)
{
    // In Visual mode and typing "gUU" triggers an operator
    if (cap->oap->op_type == OP_UPPER || VIsual_active)
    {
	// translate "gUU" to "gUgU"
	cap->cmdchar = 'g';
	cap->nchar = 'U';
	nv_operator(cap);
    }
    else if (!checkclearopq(cap->oap))
    {
	u_undoline();
	curwin->w_set_curswant = TRUE;
    }
}