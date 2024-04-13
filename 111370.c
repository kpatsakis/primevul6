static enum TIFFReadDirEntryErr TIFFReadDirEntryIfd8Array(TIFF* tif, TIFFDirEntry* direntry, uint64** value)
{
	enum TIFFReadDirEntryErr err;
	uint32 count;
	void* origdata;
	uint64* data;
	switch (direntry->tdir_type)
	{
		case TIFF_LONG:
		case TIFF_LONG8:
		case TIFF_IFD:
		case TIFF_IFD8:
			break;
		default:
			return(TIFFReadDirEntryErrType);
	}
	err=TIFFReadDirEntryArray(tif,direntry,&count,8,&origdata);
	if ((err!=TIFFReadDirEntryErrOk)||(origdata==0))
	{
		*value=0;
		return(err);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_LONG8:
		case TIFF_IFD8:
			*value=(uint64*)origdata;
			if (tif->tif_flags&TIFF_SWAB)
				TIFFSwabArrayOfLong8(*value,count);
			return(TIFFReadDirEntryErrOk);
	}
	data=(uint64*)_TIFFmalloc(count*8);
	if (data==0)
	{
		_TIFFfree(origdata);
		return(TIFFReadDirEntryErrAlloc);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_LONG:
		case TIFF_IFD:
			{
				uint32* ma;
				uint64* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					*mb++=(uint64)(*ma++);
				}
			}
			break;
	}
	_TIFFfree(origdata);
	*value=data;
	return(TIFFReadDirEntryErrOk);
}