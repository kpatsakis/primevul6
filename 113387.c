void S_AL_StopCapture( void )
{
	if (alCaptureDevice != NULL)
		qalcCaptureStop(alCaptureDevice);
}