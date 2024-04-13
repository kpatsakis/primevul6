static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeByteSshort(int16 value)
{
	if ((value<0)||(value>0xFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}