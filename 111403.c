TIFFFetchNormalTag(TIFF* tif, TIFFDirEntry* dp, int recover)
{
	static const char module[] = "TIFFFetchNormalTag";
	enum TIFFReadDirEntryErr err;
	uint32 fii;
	const TIFFField* fip = NULL;
	TIFFReadDirectoryFindFieldInfo(tif,dp->tdir_tag,&fii);
        if( fii == FAILED_FII )
        {
            TIFFErrorExt(tif->tif_clientdata, "TIFFFetchNormalTag",
                         "No definition found for tag %d",
                         dp->tdir_tag);
            return 0;
        }
	fip=tif->tif_fields[fii];
	assert(fip != NULL); /* should not happen */
	assert(fip->set_field_type!=TIFF_SETGET_OTHER);  /* if so, we shouldn't arrive here but deal with this in specialized code */
	assert(fip->set_field_type!=TIFF_SETGET_INT);    /* if so, we shouldn't arrive here as this is only the case for pseudo-tags */
	err=TIFFReadDirEntryErrOk;
	switch (fip->set_field_type)
	{
		case TIFF_SETGET_UNDEFINED:
			break;
		case TIFF_SETGET_ASCII:
			{
				uint8* data;
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryByteArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					uint8* ma;
					uint32 mb;
					int n;
					ma=data;
					mb=0;
					while (mb<(uint32)dp->tdir_count)
					{
						if (*ma==0)
							break;
						ma++;
						mb++;
					}
					if (mb+1<(uint32)dp->tdir_count)
						TIFFWarningExt(tif->tif_clientdata,module,"ASCII value for tag \"%s\" contains null byte in value; value incorrectly truncated during reading due to implementation limitations",fip->field_name);
					else if (mb+1>(uint32)dp->tdir_count)
					{
						uint8* o;
						TIFFWarningExt(tif->tif_clientdata,module,"ASCII value for tag \"%s\" does not end in null byte",fip->field_name);
						if ((uint32)dp->tdir_count+1!=dp->tdir_count+1)
							o=NULL;
						else
							o=_TIFFmalloc((uint32)dp->tdir_count+1);
						if (o==NULL)
						{
							if (data!=NULL)
								_TIFFfree(data);
							return(0);
						}
						_TIFFmemcpy(o,data,(uint32)dp->tdir_count);
						o[(uint32)dp->tdir_count]=0;
						if (data!=0)
							_TIFFfree(data);
						data=o;
					}
					n=TIFFSetField(tif,dp->tdir_tag,data);
					if (data!=0)
						_TIFFfree(data);
					if (!n)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_UINT8:
			{
				uint8 data=0;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryByte(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_UINT16:
			{
				uint16 data;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryShort(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_UINT32:
			{
				uint32 data;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryLong(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_UINT64:
			{
				uint64 data;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryLong8(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_FLOAT:
			{
				float data;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryFloat(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_DOUBLE:
			{
				double data;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryDouble(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_IFD8:
			{
				uint64 data;
				assert(fip->field_readcount==1);
				assert(fip->field_passcount==0);
				err=TIFFReadDirEntryIfd8(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					if (!TIFFSetField(tif,dp->tdir_tag,data))
						return(0);
				}
			}
			break;
		case TIFF_SETGET_UINT16_PAIR:
			{
				uint16* data;
				assert(fip->field_readcount==2);
				assert(fip->field_passcount==0);
				if (dp->tdir_count!=2) {
					TIFFWarningExt(tif->tif_clientdata,module,
						       "incorrect count for field \"%s\", expected 2, got %d",
						       fip->field_name,(int)dp->tdir_count);
					return(0);
				}
				err=TIFFReadDirEntryShortArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,data[0],data[1]);
					_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C0_UINT8:
			{
				uint8* data;
				assert(fip->field_readcount>=1);
				assert(fip->field_passcount==0);
				if (dp->tdir_count!=(uint64)fip->field_readcount) {
					TIFFWarningExt(tif->tif_clientdata,module,
						       "incorrect count for field \"%s\", expected %d, got %d",
						       fip->field_name,(int) fip->field_readcount, (int)dp->tdir_count);
					return 0;
				}
				else
				{
					err=TIFFReadDirEntryByteArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C0_UINT16:
			{
				uint16* data;
				assert(fip->field_readcount>=1);
				assert(fip->field_passcount==0);
				if (dp->tdir_count!=(uint64)fip->field_readcount)
                                    /* corrupt file */;
				else
				{
					err=TIFFReadDirEntryShortArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C0_UINT32:
			{
				uint32* data;
				assert(fip->field_readcount>=1);
				assert(fip->field_passcount==0);
				if (dp->tdir_count!=(uint64)fip->field_readcount)
                                    /* corrupt file */;
				else
				{
					err=TIFFReadDirEntryLongArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C0_FLOAT:
			{
				float* data;
				assert(fip->field_readcount>=1);
				assert(fip->field_passcount==0);
				if (dp->tdir_count!=(uint64)fip->field_readcount)
                                    /* corrupt file */;
				else
				{
					err=TIFFReadDirEntryFloatArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_ASCII:
			{
				uint8* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryByteArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
                        if( dp->tdir_count > 0 && data[dp->tdir_count-1] != '\0' )
                        {
                            TIFFWarningExt(tif->tif_clientdata,module,"ASCII value for tag \"%s\" does not end in null byte. Forcing it to be null",fip->field_name);
                            data[dp->tdir_count-1] = '\0';
                        }
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_UINT8:
			{
				uint8* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryByteArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_UINT16:
			{
				uint16* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryShortArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_UINT32:
			{
				uint32* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryLongArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_UINT64:
			{
				uint64* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryLong8Array(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_FLOAT:
			{
				float* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryFloatArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_DOUBLE:
			{
				double* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryDoubleArray(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C16_IFD8:
			{
				uint64* data;
				assert(fip->field_readcount==TIFF_VARIABLE);
				assert(fip->field_passcount==1);
				if (dp->tdir_count>0xFFFF)
					err=TIFFReadDirEntryErrCount;
				else
				{
					err=TIFFReadDirEntryIfd8Array(tif,dp,&data);
					if (err==TIFFReadDirEntryErrOk)
					{
						int m;
						m=TIFFSetField(tif,dp->tdir_tag,(uint16)(dp->tdir_count),data);
						if (data!=0)
							_TIFFfree(data);
						if (!m)
							return(0);
					}
				}
			}
			break;
		case TIFF_SETGET_C32_ASCII:
			{
				uint8* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryByteArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
                    if( dp->tdir_count > 0 && data[dp->tdir_count-1] != '\0' )
                    {
                        TIFFWarningExt(tif->tif_clientdata,module,"ASCII value for tag \"%s\" does not end in null byte. Forcing it to be null",fip->field_name);
                        data[dp->tdir_count-1] = '\0';
                    }
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_UINT8:
			{
				uint8* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryByteArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_SINT8:
			{
				int8* data = NULL;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntrySbyteArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_UINT16:
			{
				uint16* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryShortArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_SINT16:
			{
				int16* data = NULL;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntrySshortArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_UINT32:
			{
				uint32* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryLongArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_SINT32:
			{
				int32* data = NULL;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntrySlongArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_UINT64:
			{
				uint64* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryLong8Array(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_SINT64:
			{
				int64* data = NULL;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntrySlong8Array(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_FLOAT:
			{
				float* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryFloatArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_DOUBLE:
			{
				double* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryDoubleArray(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		case TIFF_SETGET_C32_IFD8:
			{
				uint64* data;
				assert(fip->field_readcount==TIFF_VARIABLE2);
				assert(fip->field_passcount==1);
				err=TIFFReadDirEntryIfd8Array(tif,dp,&data);
				if (err==TIFFReadDirEntryErrOk)
				{
					int m;
					m=TIFFSetField(tif,dp->tdir_tag,(uint32)(dp->tdir_count),data);
					if (data!=0)
						_TIFFfree(data);
					if (!m)
						return(0);
				}
			}
			break;
		default:
			assert(0);    /* we should never get here */
			break;
	}
	if (err!=TIFFReadDirEntryErrOk)
	{
		TIFFReadDirEntryOutputErr(tif,err,module,fip->field_name,recover);
		return(0);
	}
	return(1);
}