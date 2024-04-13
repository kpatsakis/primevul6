static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeByteSlong8(int64 value)
{
	if ((value<0)||(value>0xFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}