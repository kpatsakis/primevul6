decompile5Action(int n, SWF_ACTION *actions,int indent)
{
	int j;
	if( !n  )
		return NULL;

	pool = NULL;
	poolcounter = 0;

	dcinit();

	for(j=0;j<256;j++) regs[j]=0;
	regs[1] = newVar("R1");
	regs[2] = newVar("R2");
	regs[3] = newVar("R3");
	regs[4] = newVar("R4");

	decompileActions(n, actions, indent);
#ifdef DEBUGSTACK
	if( Stack != NULL && *dcstr) 
	{ 
		int i=0;
		println("/* -----------------------------------------------------------------");
		println("NOTE: some stuff left on the stack at the end of a block of actions:");
		while (Stack)
		{
			i++;
			printf("%d.:\t%s",i, getString(pop()));
			println("");
		}
		println("*/");
	}
#else
	if( Stack != NULL ) 
		fprintf(stderr,
		"Stuff left on the stack at the end of a block of actions!?!?!?\n");
	while (Stack)
	{
		pop();
	}
#endif
	return dcgetstr();
}