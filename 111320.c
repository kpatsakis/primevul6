static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSbyteLong(uint32 value)
{
	if (value>0x7F)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}