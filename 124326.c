decompileWAITFORFRAME (SWF_ACTION *act)
{
	OUT_BEGIN(SWF_ACTIONWAITFORFRAME);

	INDENT
	println("WaitForFrame(%d,%d);", sact->Frame,sact->SkipCount);
}