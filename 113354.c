qboolean S_AL_SrcInit( void )
{
	int i;
	int limit;

	// Clear the sources data structure
	memset(srcList, 0, sizeof(srcList));
	srcCount = 0;
	srcActiveCnt = 0;

	// Cap s_alSources to MAX_SRC
	limit = s_alSources->integer;
	if(limit > MAX_SRC)
		limit = MAX_SRC;
	else if(limit < 16)
		limit = 16;
 
	S_AL_ClearError( qfalse );
	// Allocate as many sources as possible
	for(i = 0; i < limit; i++)
	{
		qalGenSources(1, &srcList[i].alSource);
		if(qalGetError() != AL_NO_ERROR)
			break;
		srcCount++;
	}

	// All done. Print this for informational purposes
	Com_Printf( "Allocated %d sources.\n", srcCount);
	alSourcesInitialised = qtrue;
	return qtrue;
}