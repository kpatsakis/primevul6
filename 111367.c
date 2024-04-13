static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeByteSlong(int32 value)
{
	if ((value<0)||(value>0xFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}