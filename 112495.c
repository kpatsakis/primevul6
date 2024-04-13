status WAVEFile::parseData(const Tag &id, uint32_t size)
{
	Track *track = getTrack();

	track->fpos_first_frame = m_fh->tell();
	track->data_size = size;

	return AF_SUCCEED;
}