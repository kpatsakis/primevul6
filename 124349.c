decompileSETTARGET(int n, SWF_ACTION *actions, int maxn, int is_type2)
{
	int action_cnt=0;
	char *name;
	OUT_BEGIN2(SWF_ACTIONSETTARGET);
	name = is_type2 ? getString(pop()) : sact->TargetName;
	if (*name)
	{
		INDENT
		println("tellTarget('%s') {" ,name);
		while(action_cnt+n<maxn)
		{
			if (OpCode(actions, n+1+action_cnt, maxn)==SWFACTION_SETTARGET
			    || OpCode(actions, n+1+action_cnt, maxn)==SWFACTION_SETTARGET2
			    || OpCode(actions, n+1+action_cnt, maxn)==SWFACTION_DEFINEFUNCTION
			    || OpCode(actions, n+1+action_cnt, maxn)==SWFACTION_DEFINEFUNCTION2
			    || OpCode(actions, n+1+action_cnt, maxn)==SWFACTION_END) 
			{
				break;
			}
			action_cnt++;
		}
		decompileActions(action_cnt,&actions[n+1],gIndent+1);
		INDENT
		println("}" );
	}
	return action_cnt;
}