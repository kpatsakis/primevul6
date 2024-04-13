static enum TIFFReadDirEntryErr TIFFReadDirEntryShortArray(TIFF* tif, TIFFDirEntry* direntry, uint16** value)
{
	enum TIFFReadDirEntryErr err;
	uint32 count;
	void* origdata;
	uint16* data;
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
	err=TIFFReadDirEntryArray(tif,direntry,&count,2,&origdata);
	if ((err!=TIFFReadDirEntryErrOk)||(origdata==0))
	{
		*value=0;
		return(err);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_SHORT:
			*value=(uint16*)origdata;
			if (tif->tif_flags&TIFF_SWAB)
				TIFFSwabArrayOfShort(*value,count);  
			return(TIFFReadDirEntryErrOk);
		case TIFF_SSHORT:
			{
				int16* m;
				uint32 n;
				m=(int16*)origdata;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort((uint16*)m);
					err=TIFFReadDirEntryCheckRangeShortSshort(*m);
					if (err!=TIFFReadDirEntryErrOk)
					{
						_TIFFfree(origdata);
						return(err);
					}
					m++;
				}
				*value=(uint16*)origdata;
				return(TIFFReadDirEntryErrOk);
			}
	}
	data=(uint16*)_TIFFmalloc(count*2);
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
				uint16* mb;
				uint32 n;
				ma=(uint8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(uint16)(*ma++);
			}
			break;
		case TIFF_SBYTE:
			{
				int8* ma;
				uint16* mb;
				uint32 n;
				ma=(int8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					err=TIFFReadDirEntryCheckRangeShortSbyte(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint16)(*ma++);
				}
			}
			break;
		case TIFF_LONG:
			{
				uint32* ma;
				uint16* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					err=TIFFReadDirEntryCheckRangeShortLong(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint16)(*ma++);
				}
			}
			break;
		case TIFF_SLONG:
			{
				int32* ma;
				uint16* mb;
				uint32 n;
				ma=(int32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong((uint32*)ma);
					err=TIFFReadDirEntryCheckRangeShortSlong(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint16)(*ma++);
				}
			}
			break;
		case TIFF_LONG8:
			{
				uint64* ma;
				uint16* mb;
				uint32 n;
				ma=(uint64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8(ma);
					err=TIFFReadDirEntryCheckRangeShortLong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint16)(*ma++);
				}
			}
			break;
		case TIFF_SLONG8:
			{
				int64* ma;
				uint16* mb;
				uint32 n;
				ma=(int64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8((uint64*)ma);
					err=TIFFReadDirEntryCheckRangeShortSlong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint16)(*ma++);
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