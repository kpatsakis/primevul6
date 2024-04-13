TIFFReadDirEntryDoubleArray(TIFF* tif, TIFFDirEntry* direntry, double** value)
{
	enum TIFFReadDirEntryErr err;
	uint32 count;
	void* origdata;
	double* data;
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
		case TIFF_RATIONAL:
		case TIFF_SRATIONAL:
		case TIFF_FLOAT:
		case TIFF_DOUBLE:
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
		case TIFF_DOUBLE:
			if (tif->tif_flags&TIFF_SWAB)
				TIFFSwabArrayOfLong8((uint64*)origdata,count);
			TIFFCvtIEEEDoubleToNative(tif,count,(double*)origdata);
			*value=(double*)origdata;
			return(TIFFReadDirEntryErrOk);
	}
	data=(double*)_TIFFmalloc(count*sizeof(double));
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
				double* mb;
				uint32 n;
				ma=(uint8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(double)(*ma++);
			}
			break;
		case TIFF_SBYTE:
			{
				int8* ma;
				double* mb;
				uint32 n;
				ma=(int8*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(double)(*ma++);
			}
			break;
		case TIFF_SHORT:
			{
				uint16* ma;
				double* mb;
				uint32 n;
				ma=(uint16*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort(ma);
					*mb++=(double)(*ma++);
				}
			}
			break;
		case TIFF_SSHORT:
			{
				int16* ma;
				double* mb;
				uint32 n;
				ma=(int16*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabShort((uint16*)ma);
					*mb++=(double)(*ma++);
				}
			}
			break;
		case TIFF_LONG:
			{
				uint32* ma;
				double* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					*mb++=(double)(*ma++);
				}
			}
			break;
		case TIFF_SLONG:
			{
				int32* ma;
				double* mb;
				uint32 n;
				ma=(int32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong((uint32*)ma);
					*mb++=(double)(*ma++);
				}
			}
			break;
		case TIFF_LONG8:
			{
				uint64* ma;
				double* mb;
				uint32 n;
				ma=(uint64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8(ma);
#if defined(__WIN32__) && (_MSC_VER < 1500)
					/*
					 * XXX: MSVC 6.0 does not support
					 * conversion of 64-bit integers into
					 * floating point values.
					 */
					*mb++ = _TIFFUInt64ToDouble(*ma++);
#else
					*mb++ = (double)(*ma++);
#endif
				}
			}
			break;
		case TIFF_SLONG8:
			{
				int64* ma;
				double* mb;
				uint32 n;
				ma=(int64*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong8((uint64*)ma);
					*mb++=(double)(*ma++);
				}
			}
			break;
		case TIFF_RATIONAL:
			{
				uint32* ma;
				uint32 maa;
				uint32 mab;
				double* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					maa=*ma++;
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					mab=*ma++;
					if (mab==0)
						*mb++=0.0;
					else
						*mb++=(double)maa/(double)mab;
				}
			}
			break;
		case TIFF_SRATIONAL:
			{
				uint32* ma;
				int32 maa;
				uint32 mab;
				double* mb;
				uint32 n;
				ma=(uint32*)origdata;
				mb=data;
				for (n=0; n<count; n++)
				{
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					maa=*(int32*)ma;
					ma++;
					if (tif->tif_flags&TIFF_SWAB)
						TIFFSwabLong(ma);
					mab=*ma++;
					if (mab==0)
						*mb++=0.0;
					else
						*mb++=(double)maa/(double)mab;
				}
			}
			break;
		case TIFF_FLOAT:
			{
				float* ma;
				double* mb;
				uint32 n;
				if (tif->tif_flags&TIFF_SWAB)
					TIFFSwabArrayOfLong((uint32*)origdata,count);  
				TIFFCvtIEEEFloatToNative(tif,count,(float*)origdata);
				ma=(float*)origdata;
				mb=data;
				for (n=0; n<count; n++)
					*mb++=(double)(*ma++);
			}
			break;
	}
	_TIFFfree(origdata);
	*value=data;
	return(TIFFReadDirEntryErrOk);
}