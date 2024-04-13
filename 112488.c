void WAVEFile::initCompressionParams()
{
	Track *track = getTrack();
	if (track->f.compressionType == AF_COMPRESSION_IMA)
		initIMACompressionParams();
	else if (track->f.compressionType == AF_COMPRESSION_MS_ADPCM)
		initMSADPCMCompressionParams();
}