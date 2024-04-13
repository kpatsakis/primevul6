static enum TIFFReadDirEntryErr TIFFReadDirEntrySlongArray(TIFF* tif, TIFFDirEntry* direntry, int32** value)
{
	enum TIFFReadDirEntryErr err;
	uint32 count;
	void* origdata;
	int32* data;
	switch (direntry->tdir_type)
	{
		case TIFF_BYTE:
		case TIFF_SBYTE:
		case TIFF_SHORT:
		case TIFF_SSHORT:
		case TIFF_LONG:
		case TIFF_SLONG:
		case TIFF_LONG8:
		case TIFF_SLONG8:
			break;
		default:
			return(TIFFReadDirEntryErrType);
	}
	err=TIFFReadDirEntryArray(tif,direntry,&count,4,&origdata);
	if ((err!=TIFFReadDirEntryErrOk)||(origdata==0))
	{
		*value=0;
		return(err);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_LONG:
			{
				uint32* m;
				uint32 n;
				m=(uint32*)origdata;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong((uint32*)m);
					err=TIFFReadDirEntryCheckRangeSlongLong(*m);
					if (err!=TIFFReadDirEntryErrOk)
					{
						_TIFFfree(origdata);
						return(err);
					}
					m++;
				}
				*value=(int32*)origdata;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SLONG:
			*value=(int32*)origdata;
			if (tif->tif_flags&TIFF_SWAB)
				TIFFSwabArrayOfLong((uint32*)(*value),count);
			return(TIFFReadDirEntryErrOk);
	}
	data=(int32*)_TIFFmalloc(count*4);
	if (data==0)
	{
		_TIFFfree(origdata);
		return(TIFFReadDirEntryErrAlloc);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_BYTE:
			{
				uint8* ma;
				int32* mb;
				uint32 n;
				ma=(uint8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(int32)(*ma++);
			}
			break;
		case TIFF_SBYTE:
			{
				int8* ma;
				int32* mb;
				uint32 n;
				ma=(int8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(int32)(*ma++);
			}
			break;
		case TIFF_SHORT:
			{
				uint16* ma;
				int32* mb;
				uint32 n;
				ma=(uint16*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort(ma);
					*mb++=(int32)(*ma++);
				}
			}
			break;
		case TIFF_SSHORT:
			{
				int16* ma;
				int32* mb;
				uint32 n;
				ma=(int16*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort((uint16*)ma);
					*mb++=(int32)(*ma++);
				}
			}
			break;
		case TIFF_LONG8:
			{
				uint64* ma;
				int32* mb;
				uint32 n;
				ma=(uint64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8(ma);
					err=TIFFReadDirEntryCheckRangeSlongLong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(int32)(*ma++);
				}
			}
			break;
		case TIFF_SLONG8:
			{
				int64* ma;
				int32* mb;
				uint32 n;
				ma=(int64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8((uint64*)ma);
					err=TIFFReadDirEntryCheckRangeSlongSlong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(int32)(*ma++);
				}
			}
			break;
	}
	_TIFFfree(origdata);
	if (err!=TIFFReadDirEntryErrOk)
	{
		_TIFFfree(data);
		return(err);
	}
	*value=data;
	return(TIFFReadDirEntryErrOk);
}