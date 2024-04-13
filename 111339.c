static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSbyteLong8(uint64 value)
{
	if (value>0x7F)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}