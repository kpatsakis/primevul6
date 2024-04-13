countAllSwitchActions (union SWF_ACTION *actions, union SWF_ACTION *pre)
{
	int i,j=1;
	if (actions->SWF_ACTIONRECORD.ActionCode==SWFACTION_IF && pre->SWF_ACTIONRECORD.ActionCode==SWFACTION_STRICTEQUALS )
	{
		for(i=0; i < ((struct SWF_ACTIONIF*)actions)->numActions; i++)
		{
			j+=countAllSwitchActions(&((struct SWF_ACTIONIF*)actions)->Actions[i],pre);
			pre=&((struct SWF_ACTIONIF*)actions)->Actions[i];
		}
	}                  
	return j;
}