TIFFReadDirEntryCheckRangeSlong8Long8(uint64 value)
{
	if (value > TIFF_INT64_MAX)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}