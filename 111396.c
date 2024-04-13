static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeByteLong(uint32 value)
{
	if (value>0xFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}