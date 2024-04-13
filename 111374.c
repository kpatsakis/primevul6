static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeLongSlong(int32 value)
{
	if (value<0)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}