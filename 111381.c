static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSshortLong(uint32 value)
{
	if (value>0x7FFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}