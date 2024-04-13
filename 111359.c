static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckedSlong8(TIFF* tif, TIFFDirEntry* direntry, int64* value)
{
	if (!(tif->tif_flags&TIFF_BIGTIFF))
	{
		enum TIFFReadDirEntryErr err;
		uint32 offset = direntry->tdir_offset.toff_long;
		if (tif->tif_flags&TIFF_SWAB)
			TIFFSwabLong(&offset);
		err=TIFFReadDirEntryData(tif,offset,8,value);
		if (err!=TIFFReadDirEntryErrOk)
			return(err);
	}
	else
		*value=*(int64*)(&direntry->tdir_offset);
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong8((uint64*)value);
	return(TIFFReadDirEntryErrOk);
}