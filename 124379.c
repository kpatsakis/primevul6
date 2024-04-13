decompileIMPLEMENTS(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *nparam;
	int i;
	INDENT;
	puts(getName(pop()));
	printf(" implements ");
	nparam=pop();
	for(i=0;i<nparam->p.Integer;i++) 
	{
		puts(getName(pop()));
	}
	println(" ;");
	return 0;
}