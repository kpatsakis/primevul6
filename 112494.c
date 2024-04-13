status WAVEFile::writeFrameCount()
{
	uint32_t factSize = 4;
	uint32_t totalFrameCount;

	Track *track = getTrack();

	/* Omit the fact chunk only for uncompressed integer audio formats. */
	if (track->f.compressionType == AF_COMPRESSION_NONE &&
		(track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP ||
		track->f.sampleFormat == AF_SAMPFMT_UNSIGNED))
		return AF_SUCCEED;

	/*
		If the offset for the fact chunk hasn't been set yet,
		set it to the file's current position.
	*/
	if (m_factOffset == 0)
		m_factOffset = m_fh->tell();
	else
		m_fh->seek(m_factOffset, File::SeekFromBeginning);

	m_fh->write("fact", 4);
	writeU32(&factSize);

	totalFrameCount = track->totalfframes;
	writeU32(&totalFrameCount);

	return AF_SUCCEED;
}