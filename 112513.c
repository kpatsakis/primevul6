status WAVEFile::writeCues()
{
	Track *track = getTrack();

	if (!track->markerCount)
		return AF_SUCCEED;

	if (m_markOffset == 0)
		m_markOffset = m_fh->tell();
	else
		m_fh->seek(m_markOffset, File::SeekFromBeginning);

	Tag cue("cue ");
	writeTag(&cue);

	/*
		The cue chunk consists of 4 bytes for the number of cue points
		followed by 24 bytes for each cue point record.
	*/
	uint32_t cueChunkSize = 4 + track->markerCount * 24;
	writeU32(&cueChunkSize);
	uint32_t numCues = track->markerCount;
	writeU32(&numCues);

	// Write each marker to the file.
	for (int i=0; i<track->markerCount; i++)
	{
		uint32_t identifier = track->markers[i].id;
		writeU32(&identifier);

		uint32_t position = i;
		writeU32(&position);

		Tag data("data");
		writeTag(&data);

		/*
			For an uncompressed WAVE file which contains only one data chunk,
			chunkStart and blockStart are zero.
		*/
		uint32_t chunkStart = 0;
		writeU32(&chunkStart);

		uint32_t blockStart = 0;
		writeU32(&blockStart);

		AFframecount markPosition = track->markers[i].position;
		uint32_t sampleOffset = markPosition;
		writeU32(&sampleOffset);
	}

	// Now write the cue names and comments within a master list chunk.
	uint32_t listChunkSize = 4;
	for (int i=0; i<track->markerCount; i++)
	{
		const char *name = track->markers[i].name;
		const char *comment = track->markers[i].comment;

		/*
			Each 'labl' or 'note' chunk consists of 4 bytes for the chunk ID,
			4 bytes for the chunk data size, 4 bytes for the cue point ID,
			and then the length of the label as a null-terminated string.

			In all, this is 12 bytes plus the length of the string, its null
			termination byte, and a trailing pad byte if the length of the
			chunk is otherwise odd.
		*/
		listChunkSize += 12 + zStringLength(name);
		listChunkSize += 12 + zStringLength(comment);
	}

	Tag list("LIST");
	writeTag(&list);
	writeU32(&listChunkSize);
	Tag adtl("adtl");
	writeTag(&adtl);

	for (int i=0; i<track->markerCount; i++)
	{
		uint32_t cuePointID = track->markers[i].id;

		const char *name = track->markers[i].name;
		uint32_t labelSize = 4 + zStringLength(name);
		Tag lablTag("labl");
		writeTag(&lablTag);
		writeU32(&labelSize);
		writeU32(&cuePointID);
		writeZString(name);

		const char *comment = track->markers[i].comment;
		uint32_t noteSize = 4 + zStringLength(comment);
		Tag noteTag("note");
		writeTag(&noteTag);
		writeU32(&noteSize);
		writeU32(&cuePointID);
		writeZString(comment);
	}

	return AF_SUCCEED;
}