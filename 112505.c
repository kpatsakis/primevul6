status WAVEFile::parsePlayList(const Tag &id, uint32_t size)
{
	uint32_t segmentCount;
	readU32(&segmentCount);

	if (segmentCount == 0)
	{
		m_instrumentCount = 0;
		m_instruments = NULL;
		return AF_SUCCEED;
	}

	for (unsigned segment=0; segment<segmentCount; segment++)
	{
		uint32_t startMarkID, loopLength, loopCount;

		readU32(&startMarkID);
		readU32(&loopLength);
		readU32(&loopCount);
	}

	return AF_SUCCEED;
}