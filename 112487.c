bool WAVEFile::writeZString(const char *s)
{
	ssize_t lengthPlusNull = strlen(s) + 1;
	if (m_fh->write(s, lengthPlusNull) != lengthPlusNull)
		return false;
	if (lengthPlusNull & 1)
	{
		uint8_t zero = 0;
		if (!writeU8(&zero))
			return false;
	}
	return true;
}