int S_AL_AvailableCaptureSamples( void )
{
	int retval = 0;
	if (alCaptureDevice != NULL)
	{
		ALint samples = 0;
		qalcGetIntegerv(alCaptureDevice, ALC_CAPTURE_SAMPLES, sizeof (samples), &samples);
		retval = (int) samples;
	}
	return retval;
}