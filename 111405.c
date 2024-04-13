static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeByteLong8(uint64 value)
{
	if (value>0xFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}