static enum TIFFReadDirEntryErr TIFFReadDirEntryPersampleShort(TIFF* tif, TIFFDirEntry* direntry, uint16* value)
{
	enum TIFFReadDirEntryErr err;
	uint16* m;
	uint16* na;
	uint16 nb;
	if (direntry->tdir_count<(uint64)tif->tif_dir.td_samplesperpixel)
		return(TIFFReadDirEntryErrCount);
	err=TIFFReadDirEntryShortArray(tif,direntry,&m);
	if (err!=TIFFReadDirEntryErrOk || m == NULL)
		return(err);
	na=m;
	nb=tif->tif_dir.td_samplesperpixel;
	*value=*na++;
	nb--;
	while (nb>0)
	{
		if (*na++!=*value)
		{
			err=TIFFReadDirEntryErrPsdif;
			break;
		}
		nb--;
	}
	_TIFFfree(m);
	return(err);
}