static void S_AL_CloseMusicFiles(void)
{
	if(intro_stream)
	{
		S_CodecCloseStream(intro_stream);
		intro_stream = NULL;
	}
	
	if(mus_stream)
	{
		S_CodecCloseStream(mus_stream);
		mus_stream = NULL;
	}
}