size_t WAVEFile::zStringLength(const char *s)
{
	size_t lengthPlusNull = strlen(s) + 1;
	return lengthPlusNull + (lengthPlusNull & 1);
}