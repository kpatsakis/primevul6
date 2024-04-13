srcHandle_t S_AL_SrcAlloc( alSrcPriority_t priority, int entnum, int channel )
{
	int i;
	int empty = -1;
	int weakest = -1;
	int weakest_time = Sys_Milliseconds();
	int weakest_pri = 999;
	float weakest_gain = 1000.0;
	qboolean weakest_isplaying = qtrue;
	int weakest_numloops = 0;
	src_t *curSource;

	for(i = 0; i < srcCount; i++)
	{
		curSource = &srcList[i];
		
		// If it's locked, we aren't even going to look at it
		if(curSource->isLocked)
			continue;

		// Is it empty or not?
		if(!curSource->isActive)
		{
			empty = i;
			break;
		}

		if(curSource->isPlaying)
		{
			if(weakest_isplaying && curSource->priority < priority &&
			   (curSource->priority < weakest_pri ||
			   (!curSource->isLooping && (curSource->scaleGain < weakest_gain || curSource->lastUsedTime < weakest_time))))
			{
				// If it has lower priority, is fainter or older, flag it as weak
				// the last two values are only compared if it's not a looping sound, because we want to prevent two
				// loops (loops are added EVERY frame) fighting for a slot
				weakest_pri = curSource->priority;
				weakest_time = curSource->lastUsedTime;
				weakest_gain = curSource->scaleGain;
				weakest = i;
			}
		}
		else
		{
			weakest_isplaying = qfalse;
			
			if(weakest < 0 ||
			   knownSfx[curSource->sfx].loopCnt > weakest_numloops ||
			   curSource->priority < weakest_pri ||
			   curSource->lastUsedTime < weakest_time)
			{
				// Sources currently not playing of course have lowest priority
				// also try to always keep at least one loop master for every loop sound
				weakest_pri = curSource->priority;
				weakest_time = curSource->lastUsedTime;
				weakest_numloops = knownSfx[curSource->sfx].loopCnt;
				weakest = i;
			}
		}

		// The channel system is not actually adhered to by baseq3, and not
		// implemented in snd_dma.c, so while the following is strictly correct, it
		// causes incorrect behaviour versus defacto baseq3
#if 0
		// Is it an exact match, and not on channel 0?
		if((curSource->entity == entnum) && (curSource->channel == channel) && (channel != 0))
		{
			S_AL_SrcKill(i);
			return i;
		}
#endif
	}

	if(empty == -1)
		empty = weakest;
	
	if(empty >= 0)
	{
		S_AL_SrcKill(empty);
		srcList[empty].isActive = qtrue;
		srcActiveCnt++;
	}

	return empty;
}