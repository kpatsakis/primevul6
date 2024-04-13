TIFFReadDirEntryCheckRangeSlongLong8(uint64 value)
{
	if (value > 0x7FFFFFFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}