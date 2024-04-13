bool WAVEFile::writeUUID(const UUID *u)
{
	return m_fh->write(u->data, 16) == 16;
}