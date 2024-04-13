void S_AL_SrcUnlock(srcHandle_t src)
{
	srcList[src].isLocked = qfalse;
}