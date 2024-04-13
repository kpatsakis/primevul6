void S_AL_Shutdown( void )
{
	// Shut down everything
	int i;
	for (i = 0; i < MAX_RAW_STREAMS; i++)
		S_AL_StreamDie(i);
	S_AL_StopBackgroundTrack( );
	S_AL_SrcShutdown( );
	S_AL_BufferShutdown( );

	qalcDestroyContext(alContext);
	qalcCloseDevice(alDevice);

#ifdef USE_VOIP
	if (alCaptureDevice != NULL) {
		qalcCaptureStop(alCaptureDevice);
		qalcCaptureCloseDevice(alCaptureDevice);
		alCaptureDevice = NULL;
		Com_Printf( "OpenAL capture device closed.\n" );
	}
#endif

	for (i = 0; i < MAX_RAW_STREAMS; i++) {
		streamSourceHandles[i] = -1;
		streamPlaying[i] = qfalse;
		streamSources[i] = 0;
	}

	QAL_Shutdown();
}