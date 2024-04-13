static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSshortShort(uint16 value)
{
	if (value>0x7FFF)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}