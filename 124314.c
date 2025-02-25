decompileCALLFUNCTION(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *meth, *nparam;

	SanityCheck(SWF_CALLMETHOD, OpCode(actions, n-1, maxn) == SWFACTION_PUSH,
		"CALLMETHOD not preceeded by PUSH")

	meth=pop();
	nparam=pop();
	if (nparam->p.Integer>25)
	{
		INDENT
		println("// Problem getting function arguments (%d ignored) below:",
			nparam->p.Integer);
		nparam->p.Integer=0;
	}
	push(newVar_N("","",getName(meth),"(", nparam->p.Integer,")"));
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