static enum TIFFReadDirEntryErr TIFFReadDirEntryCheckedSrational(TIFF* tif, TIFFDirEntry* direntry, double* value)
{
	UInt64Aligned_t m;
	assert(sizeof(double)==8);
	assert(sizeof(uint64)==8);
	assert(sizeof(int32)==4);
	assert(sizeof(uint32)==4);
	if (!(tif->tif_flags&TIFF_BIGTIFF))
	{
		enum TIFFReadDirEntryErr err;
		uint32 offset = direntry->tdir_offset.toff_long;
		if (tif->tif_flags&TIFF_SWAB)
			TIFFSwabLong(&offset);
		err=TIFFReadDirEntryData(tif,offset,8,m.i);
		if (err!=TIFFReadDirEntryErrOk)
			return(err);
	}
	else
		m.l=direntry->tdir_offset.toff_long8;
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabArrayOfLong(m.i,2);
        /* Not completely sure what we should do when m.i[1]==0, but some */
        /* sanitizers do not like division by 0.0: */
        /* http://bugzilla.maptools.org/show_bug.cgi?id=2644 */
	if ((int32)m.i[0]==0 || m.i[1]==0)
		*value=0.0;
	else
		*value=(double)((int32)m.i[0])/(double)m.i[1];
	return(TIFFReadDirEntryErrOk);
}