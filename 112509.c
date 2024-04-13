status WAVEFile::writeData()
{
	Track *track = getTrack();

	m_fh->write("data", 4);
	m_dataSizeOffset = m_fh->tell();

	uint32_t chunkSize = track->data_size;

	writeU32(&chunkSize);
	track->fpos_first_frame = m_fh->tell();

	return AF_SUCCEED;
}