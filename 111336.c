static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckedLong8(TIFF* tif, TIFFDirEntry* direntry, uint64* value)
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
		*value = direntry->tdir_offset.toff_long8;
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong8(value);
	return(TIFFReadDirEntryErrOk);
}