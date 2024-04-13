static enum TIFFReadDirEntryErr TIFFReadDirEntryIfd8(TIFF* tif, TIFFDirEntry* direntry, uint64* value)
{
	enum TIFFReadDirEntryErr err;
	if (direntry->tdir_count!=1)
		return(TIFFReadDirEntryErrCount);
	switch (direntry->tdir_type)
	{
		case TIFF_LONG:
		case TIFF_IFD:
			{
				uint32 m;
				TIFFReadDirEntryCheckedLong(tif,direntry,&m);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG8:
		case TIFF_IFD8:
			err=TIFFReadDirEntryCheckedLong8(tif,direntry,value);
			return(err);
		default:
			return(TIFFReadDirEntryErrType);
	}
}