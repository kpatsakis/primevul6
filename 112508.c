status WAVEFile::parseADTLSubChunk(const Tag &id, uint32_t size)
{
	Track *track = getTrack();

	AFfileoffset endPos = m_fh->tell() + size;

	while (m_fh->tell() < endPos)
	{
		Tag chunkID;
		uint32_t chunkSize;

		readTag(&chunkID);
		readU32(&chunkSize);

		if (chunkID == "labl" || chunkID == "note")
		{
			uint32_t id;
			long length=chunkSize-4;
			char *p = (char *) _af_malloc(length);

			readU32(&id);
			m_fh->read(p, length);

			Marker *marker = track->getMarker(id);

			if (marker)
			{
				if (chunkID == "labl")
				{
					free(marker->name);
					marker->name = p;
				}
				else if (chunkID == "note")
				{
					free(marker->comment);
					marker->comment = p;
				}
				else
					free(p);
			}
			else
				free(p);

			/*
				If chunkSize is odd, skip an extra byte
				at the end of the chunk.
			*/
			if ((chunkSize % 2) != 0)
				m_fh->seek(1, File::SeekFromCurrent);
		}
		else
		{
			/* If chunkSize is odd, skip an extra byte. */
			m_fh->seek(chunkSize + (chunkSize % 2), File::SeekFromCurrent);
		}
	}
	return AF_SUCCEED;
}