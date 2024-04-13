decompileDUPLICATECLIP(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *a, *b;

	INDENT
	a = pop();
	b = pop();

	puts("duplicateMovieClip(");
	puts(getString(pop()));
	puts(",");
	puts(getString(b));
	puts(",");
	puts(getString(a));
	println(");" );
	return 0;
}