decompileLogicalNot(int n, SWF_ACTION *actions, int maxn)
{
#ifdef STATEMENT_CLASS
	if(OpCode(actions, n-1, maxn) == SWFACTION_GETVARIABLE &&
	   OpCode(actions, n+1, maxn) == SWFACTION_LOGICALNOT &&
	   OpCode(actions, n+2, maxn) == SWFACTION_IF ) 
	{
		/* It's a class statement  -- skip over both NOTs */
		return 1;
	}
#endif
	if(OpCode(actions, n+1, maxn) != SWFACTION_IF )
		push(newVar2("!",getString(pop())));
	return 0;
}