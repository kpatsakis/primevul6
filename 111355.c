static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeByteShort(uint16 value)
{
	if (value>0xFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}