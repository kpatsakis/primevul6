TIFFReadDirEntryCheckRangeSlongLong(uint32 value)
{
	if (value > 0x7FFFFFFFUL)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}