static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckRangeSbyteByte(uint8 value)
{
	if (value>0x7F)
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}