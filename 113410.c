ALuint S_AL_Format(int width, int channels)
{
	ALuint format = AL_FORMAT_MONO16;

	// Work out format
	if(width == 1)
	{
		if(channels == 1)
			format = AL_FORMAT_MONO8;
		else if(channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	else if(width == 2)
	{
		if(channels == 1)
			format = AL_FORMAT_MONO16;
		else if(channels == 2)
			format = AL_FORMAT_STEREO16;
	}

	return format;
}