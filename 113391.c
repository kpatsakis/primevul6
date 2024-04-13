void S_AL_StartCapture( void )
{
	if (alCaptureDevice != NULL)
		qalcCaptureStart(alCaptureDevice);
}