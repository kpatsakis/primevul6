decompileTRACE(int n, SWF_ACTION *actions, int maxn)
{
	INDENT
	puts("trace(");
	decompilePUSHPARAM(pop(),1);
	println(");");
	return 0;
}