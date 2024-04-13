decompileGOTOFRAME2(int n, SWF_ACTION *actions, int maxn)
{
	int i=0;
	OUT_BEGIN2(SWF_ACTIONGOTOFRAME2);
	INDENT
	if (n+1 < maxn)
	{
		if (OpCode(actions, n+1, maxn) == SWFACTION_PLAY ||
		    OpCode(actions, n+1, maxn) == SWFACTION_STOP)
			i=1;
		if (OpCode(actions, n+1, maxn) == SWFACTION_PLAY)
			puts("gotoAndPlay(");
		else
		{
			if (OpCode(actions, n+1, maxn) == SWFACTION_STOP)
				puts("gotoAndStop(");
			else
			{
				if (sact->f.FlagBits.PlayFlag)
					puts("gotoAndPlay(");
				else
					puts("gotoAndStop(");
			}
		}
	}
	else 
	{
		if (sact->f.FlagBits.PlayFlag)
			puts("gotoAndPlay(");
		else
			puts("gotoAndStop(");
	}
	decompilePUSHPARAM(pop(),0);
	println(");");
	return i;
}