static enum TIFFReadDirEntryErr TIFFReadDirEntrySshortArray(TIFF* tif, TIFFDirEntry* direntry, int16** value)
{
	enum TIFFReadDirEntryErr err;
	uint32 count;
	void* origdata;
	int16* data;
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
			{
				uint16* m;
				uint32 n;
				m=(uint16*)origdata;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort(m);
					err=TIFFReadDirEntryCheckRangeSshortShort(*m);
					if (err!=TIFFReadDirEntryErrOk)
					{
						_TIFFfree(origdata);
						return(err);
					}
					m++;
				}
				*value=(int16*)origdata;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SSHORT:
			*value=(int16*)origdata;
			if (tif->tif_flags&TIFF_SWAB)
				TIFFSwabArrayOfShort((uint16*)(*value),count);
			return(TIFFReadDirEntryErrOk);
	}
	data=(int16*)_TIFFmalloc(count*2);
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
				int16* mb;
				uint32 n;
				ma=(uint8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(int16)(*ma++);
			}
			break;
		case TIFF_SBYTE:
			{
				int8* ma;
				int16* mb;
				uint32 n;
				ma=(int8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(int16)(*ma++);
			}
			break;
		case TIFF_LONG:
			{
				uint32* ma;
				int16* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					err=TIFFReadDirEntryCheckRangeSshortLong(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(int16)(*ma++);
				}
			}
			break;
		case TIFF_SLONG:
			{
				int32* ma;
				int16* mb;
				uint32 n;
				ma=(int32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong((uint32*)ma);
					err=TIFFReadDirEntryCheckRangeSshortSlong(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(int16)(*ma++);
				}
			}
			break;
		case TIFF_LONG8:
			{
				uint64* ma;
				int16* mb;
				uint32 n;
				ma=(uint64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8(ma);
					err=TIFFReadDirEntryCheckRangeSshortLong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(int16)(*ma++);
				}
			}
			break;
		case TIFF_SLONG8:
			{
				int64* ma;
				int16* mb;
				uint32 n;
				ma=(int64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8((uint64*)ma);
					err=TIFFReadDirEntryCheckRangeSshortSlong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(int16)(*ma++);
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