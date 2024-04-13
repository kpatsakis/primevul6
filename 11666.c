npush(int x)
{
    if (TPS(stack_ptr) < STACKSIZE) {
	TPS(stack)[TPS(stack_ptr)].num_type = TRUE;
	TPS(stack)[TPS(stack_ptr)].data.num = x;
	TPS(stack_ptr)++;
    } else {
	DEBUG(2, ("npush: stack overflow: %s", _nc_visbuf(TPS(tparam_base))));
	_nc_tparm_err++;
    }
}