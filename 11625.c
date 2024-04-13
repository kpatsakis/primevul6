spush(char *x)
{
    if (TPS(stack_ptr) < STACKSIZE) {
	TPS(stack)[TPS(stack_ptr)].num_type = FALSE;
	TPS(stack)[TPS(stack_ptr)].data.str = x;
	TPS(stack_ptr)++;
    } else {
	DEBUG(2, ("spush: stack overflow: %s", _nc_visbuf(TPS(tparam_base))));
	_nc_tparm_err++;
    }
}