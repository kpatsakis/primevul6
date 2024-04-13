status WAVEFile::writeMiscellaneous()
{
	if (m_miscellaneousCount != 0)
	{
		uint32_t	miscellaneousBytes;
		uint32_t 	chunkSize;

		/* Start at 12 to account for 'LIST', size, and 'INFO'. */
		miscellaneousBytes = 12;

		/* Then calculate the size of the whole INFO chunk. */
		for (int i=0; i<m_miscellaneousCount; i++)
		{
			Tag miscid;

			// Skip miscellaneous data of an unsupported type.
			if (!misc_type_to_wave(m_miscellaneous[i].type, &miscid))
				continue;

			// Account for miscellaneous type and size.
			miscellaneousBytes += 8;
			miscellaneousBytes += m_miscellaneous[i].size;

			// Add a pad byte if necessary.
			if (m_miscellaneous[i].size % 2 != 0)
				miscellaneousBytes++;

			assert(miscellaneousBytes % 2 == 0);
		}

		if (m_miscellaneousOffset == 0)
			m_miscellaneousOffset = m_fh->tell();
		else
			m_fh->seek(m_miscellaneousOffset, File::SeekFromBeginning);

		/*
			Write the data.  On the first call to this
			function (from _af_wave_write_init), the
			data won't be available, fh->seek is used to
			reserve space until the data has been provided.
			On subseuent calls to this function (from
			_af_wave_update), the data will really be written.
		*/

		/* Write 'LIST'. */
		m_fh->write("LIST", 4);

		/* Write the size of the following chunk. */
		chunkSize = miscellaneousBytes-8;
		writeU32(&chunkSize);

		/* Write 'INFO'. */
		m_fh->write("INFO", 4);

		/* Write each miscellaneous chunk. */
		for (int i=0; i<m_miscellaneousCount; i++)
		{
			uint32_t miscsize = m_miscellaneous[i].size;
			Tag miscid;

			// Skip miscellaneous data of an unsupported type.
			if (!misc_type_to_wave(m_miscellaneous[i].type, &miscid))
				continue;

			writeTag(&miscid);
			writeU32(&miscsize);
			if (m_miscellaneous[i].buffer != NULL)
			{
				uint8_t	zero = 0;

				m_fh->write(m_miscellaneous[i].buffer, m_miscellaneous[i].size);

				// Pad if necessary.
				if ((m_miscellaneous[i].size%2) != 0)
					writeU8(&zero);
			}
			else
			{
				int	size;
				size = m_miscellaneous[i].size;

				// Pad if necessary.
				if ((size % 2) != 0)
					size++;
				m_fh->seek(size, File::SeekFromCurrent);
			}
		}
	}

	return AF_SUCCEED;
}