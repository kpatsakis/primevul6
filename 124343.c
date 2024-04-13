decompileREMOVECLIP(int n, SWF_ACTION *actions, int maxn)
{
	INDENT
	puts("removeMovieClip(");
	puts(getName(pop()));
	println(");" );
	return 0;
}