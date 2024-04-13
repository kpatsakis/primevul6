static void S_AL_NewLoopMaster(src_t *rmSource, qboolean iskilled)
{
	int index;
	src_t *curSource = NULL;
	alSfx_t *curSfx;
	
	curSfx = &knownSfx[rmSource->sfx];

	if(rmSource->isPlaying)
		curSfx->loopActiveCnt--;
	if(iskilled)
		curSfx->loopCnt--;
	
	if(curSfx->loopCnt)
	{
		if(rmSource->priority == SRCPRI_ENTITY)
		{
			if(!iskilled && rmSource->isPlaying)
			{
				// only sync ambient loops...
				// It makes more sense to have sounds for weapons/projectiles unsynced
				S_AL_SaveLoopPos(rmSource, rmSource->alSource);
			}
		}
		else if(rmSource == &srcList[curSfx->masterLoopSrc])
		{
			int firstInactive = -1;

			// Only if rmSource was the master and if there are still playing loops for
			// this sound will we need to find a new master.
	
			if(iskilled || curSfx->loopActiveCnt)
			{
				for(index = 0; index < srcCount; index++)
				{
					curSource = &srcList[index];
	
					if(curSource->sfx == rmSource->sfx && curSource != rmSource &&
					   curSource->isActive && curSource->isLooping && curSource->priority == SRCPRI_AMBIENT)
					{
						if(curSource->isPlaying)
						{
							curSfx->masterLoopSrc = index;
							break;
						}
						else if(firstInactive < 0)
							firstInactive = index;
					}
				}
			}
		
			if(!curSfx->loopActiveCnt)
			{
				if(firstInactive < 0)
				{
					if(iskilled)
					{
						curSfx->masterLoopSrc = -1;
						return;
					}
					else
						curSource = rmSource;
				}
				else
					curSource = &srcList[firstInactive];

				if(rmSource->isPlaying)
				{
					// this was the last not stopped source, save last sample position + time
					S_AL_SaveLoopPos(curSource, rmSource->alSource);
				}
				else
				{
					// second case: all loops using this sound have stopped due to listener being of of range,
					// and now the inactive master gets deleted. Just move over the soundpos settings to the
					// new master.
					curSource->lastTimePos = rmSource->lastTimePos;
					curSource->lastSampleTime = rmSource->lastSampleTime;
				}
			}
		}
	}
	else
		curSfx->masterLoopSrc = -1;
}