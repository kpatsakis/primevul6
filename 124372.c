decompileCONSTANTPOOL (SWF_ACTION *act)
{
	OUT_BEGIN(SWF_ACTIONCONSTANTPOOL);
	pool=sact->ConstantPool;
	poolcounter = sact->Count;
}