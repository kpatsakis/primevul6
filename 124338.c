decompileSTRINGCONCAT(int n, SWF_ACTION *actions, int maxn)
{
	push(newVar_N("","","concat","(", 2,")"));
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