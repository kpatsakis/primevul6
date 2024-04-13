TIFFReadDirEntryCheckRangeLongLong8(uint64 value)
{
	if (value > TIFF_UINT32_MAX)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}