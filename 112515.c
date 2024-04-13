status WAVEFile::parseCues(const Tag &id, uint32_t size)
{
	Track *track = getTrack();

	uint32_t markerCount;
	readU32(&markerCount);
	track->markerCount = markerCount;

	if (markerCount == 0)
	{
		track->markers = NULL;
		return AF_SUCCEED;
	}

	if ((track->markers = _af_marker_new(markerCount)) == NULL)
		return AF_FAIL;

	for (unsigned i=0; i<markerCount; i++)
	{
		uint32_t id, position, chunkid;
		uint32_t chunkByteOffset, blockByteOffset;
		uint32_t sampleFrameOffset;
		Marker *marker = &track->markers[i];

		readU32(&id);
		readU32(&position);
		readU32(&chunkid);
		readU32(&chunkByteOffset);
		readU32(&blockByteOffset);

		/*
			sampleFrameOffset represents the position of
			the mark in units of frames.
		*/
		readU32(&sampleFrameOffset);

		marker->id = id;
		marker->position = sampleFrameOffset;
		marker->name = _af_strdup("");
		marker->comment = _af_strdup("");
	}

	return AF_SUCCEED;
}