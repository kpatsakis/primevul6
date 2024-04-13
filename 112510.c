bool WAVEFile::recognize(File *fh)
{
	uint8_t buffer[8];

	fh->seek(0, File::SeekFromBeginning);

	if (fh->read(buffer, 8) != 8 || memcmp(buffer, "RIFF", 4) != 0)
		return false;
	if (fh->read(buffer, 4) != 4 || memcmp(buffer, "WAVE", 4) != 0)
		return false;

	return true;
}