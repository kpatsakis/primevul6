static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSbyteSshort(int16 value)
{
	if ((value<-0x80)||(value>0x7F))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}