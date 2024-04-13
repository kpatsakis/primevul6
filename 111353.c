static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSshortSlong8(int64 value)
{
	if ((value<-0x8000)||(value>0x7FFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}