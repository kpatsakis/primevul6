void S_AL_StopLoopingSound(int entityNum )
{
	if(entityList[entityNum].srcAllocated)
		S_AL_SrcKill(entityList[entityNum].srcIndex);
}