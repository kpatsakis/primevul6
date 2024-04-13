static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeLongSbyte(int8 value)
{
	if (value<0)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}