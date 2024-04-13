TIFFReadCustomDirectory(TIFF* tif, toff_t diroff,
			const TIFFFieldArray* infoarray)
{
	static const char module[] = "TIFFReadCustomDirectory";
	TIFFDirEntry* dir;
	uint16 dircount;
	TIFFDirEntry* dp;
	uint16 di;
	const TIFFField* fip;
	uint32 fii;
	_TIFFSetupFields(tif, infoarray);
	dircount=TIFFFetchDirectory(tif,diroff,&dir,NULL);
	if (!dircount)
	{
		TIFFErrorExt(tif->tif_clientdata,module,
		    "Failed to read custom directory at offset " TIFF_UINT64_FORMAT,diroff);
		return 0;
	}
	TIFFFreeDirectory(tif);
	_TIFFmemset(&tif->tif_dir, 0, sizeof(TIFFDirectory));
	TIFFReadDirectoryCheckOrder(tif,dir,dircount);
	for (di=0, dp=dir; di<dircount; di++, dp++)
	{
		TIFFReadDirectoryFindFieldInfo(tif,dp->tdir_tag,&fii);
		if (fii == FAILED_FII)
		{
			TIFFWarningExt(tif->tif_clientdata, module,
			    "Unknown field with tag %d (0x%x) encountered",
			    dp->tdir_tag, dp->tdir_tag);
			if (!_TIFFMergeFields(tif, _TIFFCreateAnonField(tif,
						dp->tdir_tag,
						(TIFFDataType) dp->tdir_type),
					     1)) {
				TIFFWarningExt(tif->tif_clientdata, module,
				    "Registering anonymous field with tag %d (0x%x) failed",
				    dp->tdir_tag, dp->tdir_tag);
				dp->tdir_tag=IGNORE;
			} else {
				TIFFReadDirectoryFindFieldInfo(tif,dp->tdir_tag,&fii);
				assert( fii != FAILED_FII );
			}
		}
		if (dp->tdir_tag!=IGNORE)
		{
			fip=tif->tif_fields[fii];
			if (fip->field_bit==FIELD_IGNORE)
				dp->tdir_tag=IGNORE;
			else
			{
				/* check data type */
				while ((fip->field_type!=TIFF_ANY)&&(fip->field_type!=dp->tdir_type))
				{
					fii++;
					if ((fii==tif->tif_nfields)||
					    (tif->tif_fields[fii]->field_tag!=(uint32)dp->tdir_tag))
					{
						fii=0xFFFF;
						break;
					}
					fip=tif->tif_fields[fii];
				}
				if (fii==0xFFFF)
				{
					TIFFWarningExt(tif->tif_clientdata, module,
					    "Wrong data type %d for \"%s\"; tag ignored",
					    dp->tdir_type,fip->field_name);
					dp->tdir_tag=IGNORE;
				}
				else
				{
					/* check count if known in advance */
					if ((fip->field_readcount!=TIFF_VARIABLE)&&
					    (fip->field_readcount!=TIFF_VARIABLE2))
					{
						uint32 expected;
						if (fip->field_readcount==TIFF_SPP)
							expected=(uint32)tif->tif_dir.td_samplesperpixel;
						else
							expected=(uint32)fip->field_readcount;
						if (!CheckDirCount(tif,dp,expected))
							dp->tdir_tag=IGNORE;
					}
				}
			}
			switch (dp->tdir_tag)
			{
				case IGNORE:
					break;
				case EXIFTAG_SUBJECTDISTANCE:
					(void) TIFFFetchSubjectDistance(tif,dp);
					break;
				default:
					(void) TIFFFetchNormalTag(tif, dp, TRUE);
					break;
			}
		}
	}
	if (dir)
		_TIFFfree(dir);
	return 1;
}