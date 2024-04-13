static enum TIFFReadDirEntryErr TIFFReadDirEntryByteArray(TIFF* tif, TIFFDirEntry* direntry, uint8** value)
{
	enum TIFFReadDirEntryErr err;
	uint32 count;
	void* origdata;
	uint8* data;
	switch (direntry->tdir_type)
	{
		case TIFF_ASCII:
		case TIFF_UNDEFINED:
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
	err=TIFFReadDirEntryArray(tif,direntry,&count,1,&origdata);
	if ((err!=TIFFReadDirEntryErrOk)||(origdata==0))
	{
		*value=0;
		return(err);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_ASCII:
		case TIFF_UNDEFINED:
		case TIFF_BYTE:
			*value=(uint8*)origdata;
			return(TIFFReadDirEntryErrOk);
		case TIFF_SBYTE:
			{
				int8* m;
				uint32 n;
				m=(int8*)origdata;
				for (n=0; n<count; n++)
				{
					err=TIFFReadDirEntryCheckRangeByteSbyte(*m);
					if (err!=TIFFReadDirEntryErrOk)
					{
						_TIFFfree(origdata);
						return(err);
					}
					m++;
				}
				*value=(uint8*)origdata;
				return(TIFFReadDirEntryErrOk);
			}
	}
	data=(uint8*)_TIFFmalloc(count);
	if (data==0)
	{
		_TIFFfree(origdata);
		return(TIFFReadDirEntryErrAlloc);
	}
	switch (direntry->tdir_type)
	{
		case TIFF_SHORT:
			{
				uint16* ma;
				uint8* mb;
				uint32 n;
				ma=(uint16*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort(ma);
					err=TIFFReadDirEntryCheckRangeByteShort(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint8)(*ma++);
				}
			}
			break;
		case TIFF_SSHORT:
			{
				int16* ma;
				uint8* mb;
				uint32 n;
				ma=(int16*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort((uint16*)ma);
					err=TIFFReadDirEntryCheckRangeByteSshort(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint8)(*ma++);
				}
			}
			break;
		case TIFF_LONG:
			{
				uint32* ma;
				uint8* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					err=TIFFReadDirEntryCheckRangeByteLong(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint8)(*ma++);
				}
			}
			break;
		case TIFF_SLONG:
			{
				int32* ma;
				uint8* mb;
				uint32 n;
				ma=(int32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong((uint32*)ma);
					err=TIFFReadDirEntryCheckRangeByteSlong(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint8)(*ma++);
				}
			}
			break;
		case TIFF_LONG8:
			{
				uint64* ma;
				uint8* mb;
				uint32 n;
				ma=(uint64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8(ma);
					err=TIFFReadDirEntryCheckRangeByteLong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint8)(*ma++);
				}
			}
			break;
		case TIFF_SLONG8:
			{
				int64* ma;
				uint8* mb;
				uint32 n;
				ma=(int64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8((uint64*)ma);
					err=TIFFReadDirEntryCheckRangeByteSlong8(*ma);
					if (err!=TIFFReadDirEntryErrOk)
						break;
					*mb++=(uint8)(*ma++);
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