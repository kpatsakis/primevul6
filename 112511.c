bool WAVEFile::readUUID(UUID *u)
{
	return m_fh->read(u->data, 16) == 16;
}