static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeLongSshort(int16 value)
{
	if (value<0)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}