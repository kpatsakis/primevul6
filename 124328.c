isLogicalOp2(int n, SWF_ACTION *actions,int maxn)
{
	switch(OpCode(actions, n, maxn))
	{
	case SWFACTION_LOGICALNOT:
	case SWFACTION_PUSHDUP:
	case SWFACTION_IF:
		return 1;
	default:
		return 0;
	}
}