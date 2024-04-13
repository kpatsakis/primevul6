void S_AL_StopAllSounds( void )
{
	int i;
	S_AL_SrcShutup();
	S_AL_StopBackgroundTrack();
	for (i = 0; i < MAX_RAW_STREAMS; i++)
		S_AL_StreamDie(i);
}