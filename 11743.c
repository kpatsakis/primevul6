npop(void)
{
    int result = 0;
    if (TPS(stack_ptr) > 0) {
	TPS(stack_ptr)--;
	if (TPS(stack)[TPS(stack_ptr)].num_type)
	    result = TPS(stack)[TPS(stack_ptr)].data.num;
    } else {
	DEBUG(2, ("npop: stack underflow: %s", _nc_visbuf(TPS(tparam_base))));
	_nc_tparm_err++;
    }
    return result;
}