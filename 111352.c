static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeShortSlong8(int64 value)
{
	if ((value<0)||(value>0xFFFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}