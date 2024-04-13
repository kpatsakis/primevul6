isStoreOp(int n, SWF_ACTION *actions,int maxn)
{
	switch(OpCode(actions, n, maxn))
	{
	case SWFACTION_STOREREGISTER:
	case SWFACTION_SETVARIABLE:
	case SWFACTION_SETMEMBER:
	case SWFACTION_CASTOP:
		return 1;
	default:
		return 0;
	}
}