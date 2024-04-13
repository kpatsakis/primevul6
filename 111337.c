static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeShortSlong(int32 value)
{
	if ((value<0)||(value>0xFFFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}