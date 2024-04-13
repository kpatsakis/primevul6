static enum TIFFReadDirEntryErr TIFFReadDirEntryShort(TIFF* tif, TIFFDirEntry* direntry, uint16* value)
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
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SBYTE:
			{
				int8 m;
				TIFFReadDirEntryCheckedSbyte(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeShortSbyte(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SHORT:
			TIFFReadDirEntryCheckedShort(tif,direntry,value);
			return(TIFFReadDirEntryErrOk);
		case TIFF_SSHORT:
			{
				int16 m;
				TIFFReadDirEntryCheckedSshort(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeShortSshort(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG:
			{
				uint32 m;
				TIFFReadDirEntryCheckedLong(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeShortLong(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SLONG:
			{
				int32 m;
				TIFFReadDirEntryCheckedSlong(tif,direntry,&m);
				err=TIFFReadDirEntryCheckRangeShortSlong(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG8:
			{
				uint64 m;
				err=TIFFReadDirEntryCheckedLong8(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				err=TIFFReadDirEntryCheckRangeShortLong8(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SLONG8:
			{
				int64 m;
				err=TIFFReadDirEntryCheckedSlong8(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				err=TIFFReadDirEntryCheckRangeShortSlong8(m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(uint16)m;
				return(TIFFReadDirEntryErrOk);
			}
		default:
			return(TIFFReadDirEntryErrType);
	}
}