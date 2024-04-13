static void S_AL_Gain(ALuint source, float gainval)
{
	if(s_muted->integer)
		qalSourcef(source, AL_GAIN, 0.0f);
	else
		qalSourcef(source, AL_GAIN, gainval);
}