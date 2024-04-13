decompileDELETE(int n, SWF_ACTION *actions, int maxn, int is_type2)
{
	if (is_type2)
		push(newVar3("delete(",getName(pop()),")"));
	else
		push(newVar_N("delete(",getName(pop()),".",getName(pop()), 0,")"));


	if (OpCode(actions, n+1, maxn) == SWFACTION_POP)
	{
		/* call delete() with its args and throw away any result */
		INDENT
		puts(getName(pop()));
		println(";" );
		return 1;
	}
	return 0;
}