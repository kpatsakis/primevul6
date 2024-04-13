status WAVEFile::writeInit(AFfilesetup setup)
{
	if (initFromSetup(setup) == AF_FAIL)
		return AF_FAIL;

	initCompressionParams();

	uint32_t zero = 0;

	m_fh->seek(0, File::SeekFromBeginning);
	m_fh->write("RIFF", 4);
	m_fh->write(&zero, 4);
	m_fh->write("WAVE", 4);

	writeMiscellaneous();
	writeCues();
	writeFormat();
	writeFrameCount();
	writeData();

	return AF_SUCCEED;
}