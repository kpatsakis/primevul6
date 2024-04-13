void S_AL_ClearLoopingSounds( qboolean killall )
{
	int i;
	for(i = 0; i < srcCount; i++)
	{
		if((srcList[i].isLooping) && (srcList[i].entity != -1))
			entityList[srcList[i].entity].loopAddedThisFrame = qfalse;
	}
}