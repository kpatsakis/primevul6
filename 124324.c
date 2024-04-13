decompileGETURL2 (SWF_ACTION *act)
{
	struct SWF_ACTIONPUSHPARAM *a,*b;
	OUT_BEGIN(SWF_ACTIONGETURL2);
	INDENT

	a = pop();
	b = pop();

	if (sact->f.FlagBits.SendVarsMethod==3)
		puts("loadVariables(");
	else 
	{
		if (sact->f.FlagBits.SendVarsMethod==2)
			puts("loadVariablesNum(");
		else
		{
			if (sact->f.FlagBits.SendVarsMethod==1) 
				puts("loadMovie(");
			else
			{
				if (*getName(a)=='_')	// found a _level
					puts("loadMovieNum(");	
				else
					puts("getURL(");
			}
		}
	}
	decompilePUSHPARAM (b, 1);
	puts(",");
	decompilePUSHPARAM (a, 1);
	if  (sact->f.FlagBits.LoadVariableFlag)
		puts(",'GET'");
	if (sact->f.FlagBits.LoadTargetFlag)
		puts(",'POST'");
	println(");");
	return 0;
}