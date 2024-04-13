decompileSingleArgBuiltInFunctionCall(int n, SWF_ACTION *actions, int maxn, char *functionname)
{
	push(newVar_N("","",functionname,"(", 1,")"));
	if (OpCode(actions, n+1, maxn) == SWFACTION_POP)
	{
		/* call function and throw away any result */
		INDENT
		puts(getName(pop()));
		println(";" );
		return 1;
	}
	return 0;
}