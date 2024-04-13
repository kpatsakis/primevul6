void WAVEFile::initIMACompressionParams()
{
	Track *track = getTrack();

	track->f.framesPerPacket = 505;
	track->f.bytesPerPacket = 256 * track->f.channelCount;

	AUpvlist pv = AUpvnew(1);
	AUpvsetparam(pv, 0, _AF_IMA_ADPCM_TYPE);
	AUpvsetvaltype(pv, 0, AU_PVTYPE_LONG);
	long l = _AF_IMA_ADPCM_TYPE_WAVE;
	AUpvsetval(pv, 0, &l);

	track->f.compressionParams = pv;
}