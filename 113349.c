void S_AL_SrcShutdown( void )
{
	int i;
	src_t *curSource;

	if(!alSourcesInitialised)
		return;

	// Destroy all the sources
	for(i = 0; i < srcCount; i++)
	{
		curSource = &srcList[i];
		
		if(curSource->isLocked)
			Com_DPrintf( S_COLOR_YELLOW "WARNING: Source %d is locked\n", i);

		if(curSource->entity > 0)
			entityList[curSource->entity].srcAllocated = qfalse;

		qalSourceStop(srcList[i].alSource);
		qalDeleteSources(1, &srcList[i].alSource);
	}

	memset(srcList, 0, sizeof(srcList));

	alSourcesInitialised = qfalse;
}