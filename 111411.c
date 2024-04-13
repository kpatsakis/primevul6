static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSbyteShort(uint16 value)
{
	if (value>0x7F)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}