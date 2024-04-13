void S_AL_Update( void )
{
	int i;

	if(s_muted->modified)
	{
		// muted state changed. Let S_AL_Gain turn up all sources again.
		for(i = 0; i < srcCount; i++)
		{
			if(srcList[i].isActive)
				S_AL_Gain(srcList[i].alSource, srcList[i].scaleGain);
		}
		
		s_muted->modified = qfalse;
	}

	// Update SFX channels
	S_AL_SrcUpdate();

	// Update streams
	for (i = 0; i < MAX_RAW_STREAMS; i++)
		S_AL_StreamUpdate(i);
	S_AL_MusicUpdate();

	// Doppler
	if(s_doppler->modified)
	{
		s_alDopplerFactor->modified = qtrue;
		s_doppler->modified = qfalse;
	}

	// Doppler parameters
	if(s_alDopplerFactor->modified)
	{
		if(s_doppler->integer)
			qalDopplerFactor(s_alDopplerFactor->value);
		else
			qalDopplerFactor(0.0f);
		s_alDopplerFactor->modified = qfalse;
	}
	if(s_alDopplerSpeed->modified)
	{
		qalSpeedOfSound(s_alDopplerSpeed->value);
		s_alDopplerSpeed->modified = qfalse;
	}

	// Clear the modified flags on the other cvars
	s_alGain->modified = qfalse;
	s_volume->modified = qfalse;
	s_musicVolume->modified = qfalse;
	s_alMinDistance->modified = qfalse;
	s_alRolloff->modified = qfalse;
}