TIFFReadDirEntryCheckRangeSlongSlong8(int64 value)
{
        if ((value < 0-((int64) 0x7FFFFFFF+1)) || (value > 0x7FFFFFFF))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}