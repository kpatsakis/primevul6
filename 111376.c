static enum TIFFReadDirEntryErr TIFFReadDirEntryLong8(TIFF* tif, TIFFDirEntry* direntry, uint64* value)
{
	enum TIFFReadDirEntryErr err;
	if (direntry->tdir_count!=1)
		return(TIFFReadDirEntryErrCount);
	switch (direntry->tdir_type)
	{
		case TIFF_BYTE:
			{
				uint8 m;
				TIFFReadDirEntryCheckedByte(tif,direntry,&m);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SBYTE:
			{
				int8 m;
				TIFFReadDirEntryCheckedSbyte(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeLong8Sbyte(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SHORT:
			{
				uint16 m;
				TIFFReadDirEntryCheckedShort(tif,direntry,&m);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SSHORT:
			{
				int16 m;
				TIFFReadDirEntryCheckedSshort(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeLong8Sshort(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG:
			{
				uint32 m;
				TIFFReadDirEntryCheckedLong(tif,direntry,&m);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SLONG:
			{
				int32 m;
				TIFFReadDirEntryCheckedSlong(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeLong8Slong(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG8:
			err=TIFFReadDirEntryCheckedLong8(tif,direntry,value);
			return(err);
		case TIFF_SLONG8:
			{
				int64 m;
				err=TIFFReadDirEntryCheckedSlong8(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				err=TIFFReadDirEntryCheckRangeLong8Slong8(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint64)m;
				return(TIFFReadDirEntryErrOk);
			}
		default:
			return(TIFFReadDirEntryErrType);
	}
}