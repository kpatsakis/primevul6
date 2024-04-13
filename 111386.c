static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSshortLong8(uint64 value)
{
	if (value>0x7FFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}