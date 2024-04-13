status WAVEFile::update()
{
	Track *track = getTrack();

	if (track->fpos_first_frame != 0)
	{
		uint32_t dataLength, fileLength;

		// Update the frame count chunk if present.
		writeFrameCount();

		// Update the length of the data chunk.
		m_fh->seek(m_dataSizeOffset, File::SeekFromBeginning);
		dataLength = (uint32_t) track->data_size;
		writeU32(&dataLength);

		// Update the length of the RIFF chunk.
		fileLength = (uint32_t) m_fh->length();
		fileLength -= 8;

		m_fh->seek(4, File::SeekFromBeginning);
		writeU32(&fileLength);
	}

	/*
		Write the actual data that was set after initializing
		the miscellaneous IDs.	The size of the data will be
		unchanged.
	*/
	writeMiscellaneous();

	// Write the new positions; the size of the data will be unchanged.
	writeCues();

	return AF_SUCCEED;
}