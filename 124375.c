decompilePUSH (SWF_ACTION *act)
{
	int i;
	OUT_BEGIN(SWF_ACTIONPUSH);

	SanityCheck(SWF_PUSH,
		act->SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
		"not a PUSH")

	for(i=0;i<sact->NumParam;i++)
		push(&(sact->Params[i]));
}