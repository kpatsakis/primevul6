static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeShortLong8(uint64 value)
{
	if (value>0xFFFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}