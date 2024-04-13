TIFFReadDirEntryCheckRangeLong8Slong8(int64 value)
{
	if (value < 0)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}