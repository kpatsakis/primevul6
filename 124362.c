decompilePUSHDUP (SWF_ACTION *act)
{
	SanityCheck(SWF_PUSHDUP,
		act->SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP,
		"not a PUSHDUP")
	pushdup();
}