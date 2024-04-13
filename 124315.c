decompileGETTIME(int n, SWF_ACTION *actions, int maxn)
{
	if (OpCode(actions, n+1, maxn) == SWFACTION_POP)
	{
		INDENT
		println("getTimer();");
		return 1;
	}
	else
	{
		push(newVar("getTimer()"));
		return 0;
	}
}