status WAVEFile::readInit(AFfilesetup setup)
{
	Tag type, formtype;
	uint32_t size;
	uint32_t index = 0;

	bool hasFormat = false;
	bool hasData = false;
	bool hasFrameCount = false;

	Track *track = allocateTrack();

	m_fh->seek(0, File::SeekFromBeginning);

	readTag(&type);
	readU32(&size);
	readTag(&formtype);

	assert(type == "RIFF");
	assert(formtype == "WAVE");

	/* Include the offset of the form type. */
	index += 4;

	while (index < size)
	{
		Tag chunkid;
		uint32_t chunksize = 0;
		status result;

		readTag(&chunkid);
		readU32(&chunksize);

		if (chunkid == "fmt ")
		{
			result = parseFormat(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;

			hasFormat = true;
		}
		else if (chunkid == "data")
		{
			/* The format chunk must precede the data chunk. */
			if (!hasFormat)
			{
				_af_error(AF_BAD_HEADER, "missing format chunk in WAVE file");
				return AF_FAIL;
			}

			result = parseData(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;

			hasData = true;
		}
		else if (chunkid == "inst")
		{
			result = parseInstrument(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (chunkid == "fact")
		{
			hasFrameCount = true;
			result = parseFrameCount(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (chunkid == "cue ")
		{
			result = parseCues(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (chunkid == "LIST" || chunkid == "list")
		{
			result = parseList(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (chunkid == "INST")
		{
			result = parseInstrument(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (chunkid == "plst")
		{
			result = parsePlayList(chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}

		index += chunksize + 8;

		/* All chunks must be aligned on an even number of bytes */
		if ((index % 2) != 0)
			index++;

		m_fh->seek(index + 8, File::SeekFromBeginning);
	}

	/* The format chunk and the data chunk are required. */
	if (!hasFormat || !hasData)
	{
		return AF_FAIL;
	}

	/*
		At this point we know that the file has a format chunk and a
		data chunk, so we can assume that track->f and track->data_size
		have been initialized.
	*/
	if (!hasFrameCount)
	{
		if (track->f.bytesPerPacket && track->f.framesPerPacket)
		{
			track->computeTotalFileFrames();
		}
		else
		{
			_af_error(AF_BAD_HEADER, "Frame count required but not found");
			return AF_FAIL;
		}
	}

	return AF_SUCCEED;
}