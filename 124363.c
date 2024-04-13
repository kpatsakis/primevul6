decompileDEFINELOCAL2(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *var;

	INDENT
	var = pop();
	puts("var ");
	puts(getName(var));
	println(";");

	return 0;
}