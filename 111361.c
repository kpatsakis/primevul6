static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckedDouble(TIFF* tif, TIFFDirEntry* direntry, double* value)
{
	assert(sizeof(double)==8);
	assert(sizeof(uint64)==8);
	assert(sizeof(UInt64Aligned_t)==8);
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
	{
	       UInt64Aligned_t uint64_union;
	       uint64_union.l=direntry->tdir_offset.toff_long8;
	       *value=uint64_union.d;
	}
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong8((uint64*)value);
	return(TIFFReadDirEntryErrOk);
}