static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeShortSshort(int16 value)
{
	if (value<0)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}