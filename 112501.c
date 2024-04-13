WAVEFile::WAVEFile()
{
	setFormatByteOrder(AF_BYTEORDER_LITTLEENDIAN);

	m_factOffset = 0;
	m_miscellaneousOffset = 0;
	m_markOffset = 0;
	m_dataSizeOffset = 0;

	m_msadpcmNumCoefficients = 0;
}