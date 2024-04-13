getAllSwitchActions(union SWF_ACTION *dest, union SWF_ACTION *actions, union SWF_ACTION *pre)
{
#ifdef DEBUGSWITCH
	println("SWCODE: %p %d %s   %s",
	        dest, actions->SWF_ACTIONRECORD.Offset, 
	        actionName(actions->SWF_ACTIONRECORD.ActionCode),
	        actionName(pre->SWF_ACTIONRECORD.ActionCode));
#endif

	*dest++=*actions;
	if (actions->SWF_ACTIONRECORD.ActionCode==SWFACTION_IF 
	    && pre->SWF_ACTIONRECORD.ActionCode==SWFACTION_STRICTEQUALS )
	{
		int i;
		struct SWF_ACTIONIF *sactv2 = (struct SWF_ACTIONIF*)actions;
		for(i=0; i< sactv2->numActions; i++)
		{
			dest=getAllSwitchActions(dest,&sactv2->Actions[i],pre);
			pre=&((struct SWF_ACTIONIF*)actions)->Actions[i];
		}
	}
	return dest;
}