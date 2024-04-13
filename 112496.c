status WAVEFile::parseINFOSubChunk(const Tag &id, uint32_t size)
{
	AFfileoffset endPos = m_fh->tell() + size;

	while (m_fh->tell() < endPos)
	{
		int misctype = AF_MISC_UNRECOGNIZED;
		Tag miscid;
		uint32_t miscsize;

		readTag(&miscid);
		readU32(&miscsize);

		if (miscid == "IART")
			misctype = AF_MISC_AUTH;
		else if (miscid == "INAM")
			misctype = AF_MISC_NAME;
		else if (miscid == "ICOP")
			misctype = AF_MISC_COPY;
		else if (miscid == "ICMT")
			misctype = AF_MISC_ICMT;
		else if (miscid == "ICRD")
			misctype = AF_MISC_ICRD;
		else if (miscid == "ISFT")
			misctype = AF_MISC_ISFT;

		if (misctype != AF_MISC_UNRECOGNIZED)
		{
			char *string = (char *) _af_malloc(miscsize);

			m_fh->read(string, miscsize);

			m_miscellaneousCount++;
			m_miscellaneous = (Miscellaneous *) _af_realloc(m_miscellaneous, sizeof (Miscellaneous) * m_miscellaneousCount);

			m_miscellaneous[m_miscellaneousCount-1].id = m_miscellaneousCount;
			m_miscellaneous[m_miscellaneousCount-1].type = misctype;
			m_miscellaneous[m_miscellaneousCount-1].size = miscsize;
			m_miscellaneous[m_miscellaneousCount-1].position = 0;
			m_miscellaneous[m_miscellaneousCount-1].buffer = string;
		}
		else
		{
			m_fh->seek(miscsize, File::SeekFromCurrent);
		}

		/* Make the current position an even number of bytes.  */
		if (miscsize % 2 != 0)
			m_fh->seek(1, File::SeekFromCurrent);
	}
	return AF_SUCCEED;
}