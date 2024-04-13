status WAVEFile::parseList(const Tag &id, uint32_t size)
{
	Tag typeID;
	readTag(&typeID);
	size-=4;

	if (typeID == "adtl")
	{
		/* Handle adtl sub-chunks. */
		return parseADTLSubChunk(typeID, size);
	}
	else if (typeID == "INFO")
	{
		/* Handle INFO sub-chunks. */
		return parseINFOSubChunk(typeID, size);
	}
	else
	{
		/* Skip unhandled sub-chunks. */
		m_fh->seek(size, File::SeekFromCurrent);
		return AF_SUCCEED;
	}
	return AF_SUCCEED;
}