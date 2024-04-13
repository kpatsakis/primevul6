static void S_AL_MusicSourceGet( void )
{
	// Allocate a musicSource at high priority
	musicSourceHandle = S_AL_SrcAlloc(SRCPRI_STREAM, -2, 0);
	if(musicSourceHandle == -1)
		return;

	// Lock the musicSource so nobody else can use it, and get the raw musicSource
	S_AL_SrcLock(musicSourceHandle);
	musicSource = S_AL_SrcGet(musicSourceHandle);

	// make sure that after unmuting the S_AL_Gain in S_Update() does not turn
	// volume up prematurely for this source
	srcList[musicSourceHandle].scaleGain = 0.0f;

	// Set some musicSource parameters
	qalSource3f(musicSource, AL_POSITION,        0.0, 0.0, 0.0);
	qalSource3f(musicSource, AL_VELOCITY,        0.0, 0.0, 0.0);
	qalSource3f(musicSource, AL_DIRECTION,       0.0, 0.0, 0.0);
	qalSourcef (musicSource, AL_ROLLOFF_FACTOR,  0.0          );
	qalSourcei (musicSource, AL_SOURCE_RELATIVE, AL_TRUE      );
}