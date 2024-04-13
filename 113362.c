void S_AL_StopBackgroundTrack( void )
{
	if(!musicPlaying)
		return;

	// Stop playing
	qalSourceStop(musicSource);

	// Detach any buffers
	qalSourcei(musicSource, AL_BUFFER, 0);

	// Delete the buffers
	qalDeleteBuffers(NUM_MUSIC_BUFFERS, musicBuffers);

	// Free the musicSource
	S_AL_MusicSourceFree();

	// Unload the stream
	S_AL_CloseMusicFiles();

	musicPlaying = qfalse;
}