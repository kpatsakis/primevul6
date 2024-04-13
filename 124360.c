decompileTHROW(int n, SWF_ACTION *actions, int maxn)
{
	INDENT
	puts("throw ");
	puts(getName(pop()));
	println(";");
	return 0;
}