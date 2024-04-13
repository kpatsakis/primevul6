decompileSTACKSWAP (SWF_ACTION *act)
{
	SanityCheck(SWF_STACKSWAP,
		act->SWF_ACTIONRECORD.ActionCode == SWFACTION_STACKSWAP,
		"not a STACKSWAP")
	stackswap();
}