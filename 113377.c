void S_AL_Capture( int samples, byte *data )
{
	if (alCaptureDevice != NULL)
		qalcCaptureSamples(alCaptureDevice, data, samples);
}