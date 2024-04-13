void WAVEFile::initMSADPCMCompressionParams()
{
	const int16_t coefficients[7][2] =
	{
		{ 256, 0 },
		{ 512, -256 },
		{ 0, 0 },
		{ 192, 64 },
		{ 240, 0 },
		{ 460, -208 },
		{ 392, -232 }
	};
	memcpy(m_msadpcmCoefficients, coefficients, sizeof (int16_t) * 7 * 2);
	m_msadpcmNumCoefficients = 7;

	Track *track = getTrack();

	track->f.framesPerPacket = 500;
	track->f.bytesPerPacket = 256 * track->f.channelCount;

	AUpvlist pv = AUpvnew(2);
	AUpvsetparam(pv, 0, _AF_MS_ADPCM_NUM_COEFFICIENTS);
	AUpvsetvaltype(pv, 0, AU_PVTYPE_LONG);
	long l = m_msadpcmNumCoefficients;
	AUpvsetval(pv, 0, &l);

	AUpvsetparam(pv, 1, _AF_MS_ADPCM_COEFFICIENTS);
	AUpvsetvaltype(pv, 1, AU_PVTYPE_PTR);
	void *v = m_msadpcmCoefficients;
	AUpvsetval(pv, 1, &v);

	track->f.compressionParams = pv;
}