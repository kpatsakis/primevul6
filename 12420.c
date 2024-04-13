set_op_var(int optype)
{
    char_u	opchars[3];

    if (optype == OP_NOP)
	set_vim_var_string(VV_OP, NULL, 0);
    else
    {
	opchars[0] = get_op_char(optype);
	opchars[1] = get_extra_op_char(optype);
	opchars[2] = NUL;
	set_vim_var_string(VV_OP, opchars, -1);
    }
}