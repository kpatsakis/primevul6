TIFFReadDirectory(TIFF* tif)
{
	static const char module[] = "TIFFReadDirectory";
	TIFFDirEntry* dir;
	uint16 dircount;
	TIFFDirEntry* dp;
	uint16 di;
	const TIFFField* fip;
	uint32 fii=FAILED_FII;
        toff_t nextdiroff;
    int bitspersample_read = FALSE;
        int color_channels;

	tif->tif_diroff=tif->tif_nextdiroff;
	if (!TIFFCheckDirOffset(tif,tif->tif_nextdiroff))
		return 0;           /* last offset or bad offset (IFD looping) */
	(*tif->tif_cleanup)(tif);   /* cleanup any previous compression state */
	tif->tif_curdir++;
        nextdiroff = tif->tif_nextdiroff;
	dircount=TIFFFetchDirectory(tif,nextdiroff,&dir,&tif->tif_nextdiroff);
	if (!dircount)
	{
		TIFFErrorExt(tif->tif_clientdata,module,
		    "Failed to read directory at offset " TIFF_UINT64_FORMAT,nextdiroff);
		return 0;
	}
	TIFFReadDirectoryCheckOrder(tif,dir,dircount);

        /*
         * Mark duplicates of any tag to be ignored (bugzilla 1994)
         * to avoid certain pathological problems.
         */
	{
		TIFFDirEntry* ma;
		uint16 mb;
		for (ma=dir, mb=0; mb<dircount; ma++, mb++)
		{
			TIFFDirEntry* na;
			uint16 nb;
			for (na=ma+1, nb=mb+1; nb<dircount; na++, nb++)
			{
				if (ma->tdir_tag==na->tdir_tag)
					na->tdir_tag=IGNORE;
			}
		}
	}
        
	tif->tif_flags &= ~TIFF_BEENWRITING;    /* reset before new dir */
	tif->tif_flags &= ~TIFF_BUF4WRITE;      /* reset before new dir */
	/* free any old stuff and reinit */
	TIFFFreeDirectory(tif);
	TIFFDefaultDirectory(tif);
	/*
	 * Electronic Arts writes gray-scale TIFF files
	 * without a PlanarConfiguration directory entry.
	 * Thus we setup a default value here, even though
	 * the TIFF spec says there is no default value.
	 */
	TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
	/*
	 * Setup default value and then make a pass over
	 * the fields to check type and tag information,
	 * and to extract info required to size data
	 * structures.  A second pass is made afterwards
	 * to read in everything not taken in the first pass.
	 * But we must process the Compression tag first
	 * in order to merge in codec-private tag definitions (otherwise
	 * we may get complaints about unknown tags).  However, the
	 * Compression tag may be dependent on the SamplesPerPixel
	 * tag value because older TIFF specs permitted Compression
	 * to be written as a SamplesPerPixel-count tag entry.
	 * Thus if we don't first figure out the correct SamplesPerPixel
	 * tag value then we may end up ignoring the Compression tag
	 * value because it has an incorrect count value (if the
	 * true value of SamplesPerPixel is not 1).
	 */
	dp=TIFFReadDirectoryFindEntry(tif,dir,dircount,TIFFTAG_SAMPLESPERPIXEL);
	if (dp)
	{
		if (!TIFFFetchNormalTag(tif,dp,0))
			goto bad;
		dp->tdir_tag=IGNORE;
	}
	dp=TIFFReadDirectoryFindEntry(tif,dir,dircount,TIFFTAG_COMPRESSION);
	if (dp)
	{
		/*
		 * The 5.0 spec says the Compression tag has one value, while
		 * earlier specs say it has one value per sample.  Because of
		 * this, we accept the tag if one value is supplied with either
		 * count.
		 */
		uint16 value;
		enum TIFFReadDirEntryErr err;
		err=TIFFReadDirEntryShort(tif,dp,&value);
		if (err==TIFFReadDirEntryErrCount)
			err=TIFFReadDirEntryPersampleShort(tif,dp,&value);
		if (err!=TIFFReadDirEntryErrOk)
		{
			TIFFReadDirEntryOutputErr(tif,err,module,"Compression",0);
			goto bad;
		}
		if (!TIFFSetField(tif,TIFFTAG_COMPRESSION,value))
			goto bad;
		dp->tdir_tag=IGNORE;
	}
	else
	{
		if (!TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE))
			goto bad;
	}
	/*
	 * First real pass over the directory.
	 */
	for (di=0, dp=dir; di<dircount; di++, dp++)
	{
		if (dp->tdir_tag!=IGNORE)
		{
			TIFFReadDirectoryFindFieldInfo(tif,dp->tdir_tag,&fii);
			if (fii == FAILED_FII)
			{
				TIFFWarningExt(tif->tif_clientdata, module,
				    "Unknown field with tag %d (0x%x) encountered",
				    dp->tdir_tag,dp->tdir_tag);
                                /* the following knowingly leaks the 
                                   anonymous field structure */
				if (!_TIFFMergeFields(tif,
					_TIFFCreateAnonField(tif,
						dp->tdir_tag,
						(TIFFDataType) dp->tdir_type),
					1)) {
					TIFFWarningExt(tif->tif_clientdata,
					    module,
					    "Registering anonymous field with tag %d (0x%x) failed",
					    dp->tdir_tag,
					    dp->tdir_tag);
					dp->tdir_tag=IGNORE;
				} else {
					TIFFReadDirectoryFindFieldInfo(tif,dp->tdir_tag,&fii);
					assert(fii != FAILED_FII);
				}
			}
		}
		if (dp->tdir_tag!=IGNORE)
		{
			fip=tif->tif_fields[fii];
			if (fip->field_bit==FIELD_IGNORE)
				dp->tdir_tag=IGNORE;
			else
			{
				switch (dp->tdir_tag)
				{
					case TIFFTAG_STRIPOFFSETS:
					case TIFFTAG_STRIPBYTECOUNTS:
					case TIFFTAG_TILEOFFSETS:
					case TIFFTAG_TILEBYTECOUNTS:
						TIFFSetFieldBit(tif,fip->field_bit);
						break;
					case TIFFTAG_IMAGEWIDTH:
					case TIFFTAG_IMAGELENGTH:
					case TIFFTAG_IMAGEDEPTH:
					case TIFFTAG_TILELENGTH:
					case TIFFTAG_TILEWIDTH:
					case TIFFTAG_TILEDEPTH:
					case TIFFTAG_PLANARCONFIG:
					case TIFFTAG_ROWSPERSTRIP:
					case TIFFTAG_EXTRASAMPLES:
						if (!TIFFFetchNormalTag(tif,dp,0))
							goto bad;
						dp->tdir_tag=IGNORE;
						break;
                                        default:
                                            if( !_TIFFCheckFieldIsValidForCodec(tif, dp->tdir_tag) )
                                                dp->tdir_tag=IGNORE;
                                            break;
				}
			}
		}
	}
	/*
	 * XXX: OJPEG hack.
	 * If a) compression is OJPEG, b) planarconfig tag says it's separate,
	 * c) strip offsets/bytecounts tag are both present and
	 * d) both contain exactly one value, then we consistently find
	 * that the buggy implementation of the buggy compression scheme
	 * matches contig planarconfig best. So we 'fix-up' the tag here
	 */
	if ((tif->tif_dir.td_compression==COMPRESSION_OJPEG)&&
	    (tif->tif_dir.td_planarconfig==PLANARCONFIG_SEPARATE))
	{
        if (!_TIFFFillStriles(tif))
            goto bad;
		dp=TIFFReadDirectoryFindEntry(tif,dir,dircount,TIFFTAG_STRIPOFFSETS);
		if ((dp!=0)&&(dp->tdir_count==1))
		{
			dp=TIFFReadDirectoryFindEntry(tif,dir,dircount,
			    TIFFTAG_STRIPBYTECOUNTS);
			if ((dp!=0)&&(dp->tdir_count==1))
			{
				tif->tif_dir.td_planarconfig=PLANARCONFIG_CONTIG;
				TIFFWarningExt(tif->tif_clientdata,module,
				    "Planarconfig tag value assumed incorrect, "
				    "assuming data is contig instead of chunky");
			}
		}
	}
	/*
	 * Allocate directory structure and setup defaults.
	 */
	if (!TIFFFieldSet(tif,FIELD_IMAGEDIMENSIONS))
	{
		MissingRequired(tif,"ImageLength");
		goto bad;
	}
	/*
	 * Setup appropriate structures (by strip or by tile)
	 */
	if (!TIFFFieldSet(tif, FIELD_TILEDIMENSIONS)) {
		tif->tif_dir.td_nstrips = TIFFNumberOfStrips(tif);  
		tif->tif_dir.td_tilewidth = tif->tif_dir.td_imagewidth;
		tif->tif_dir.td_tilelength = tif->tif_dir.td_rowsperstrip;
		tif->tif_dir.td_tiledepth = tif->tif_dir.td_imagedepth;
		tif->tif_flags &= ~TIFF_ISTILED;
	} else {
		tif->tif_dir.td_nstrips = TIFFNumberOfTiles(tif);
		tif->tif_flags |= TIFF_ISTILED;
	}
	if (!tif->tif_dir.td_nstrips) {
		TIFFErrorExt(tif->tif_clientdata, module,
		    "Cannot handle zero number of %s",
		    isTiled(tif) ? "tiles" : "strips");
		goto bad;
	}
	tif->tif_dir.td_stripsperimage = tif->tif_dir.td_nstrips;
	if (tif->tif_dir.td_planarconfig == PLANARCONFIG_SEPARATE)
		tif->tif_dir.td_stripsperimage /= tif->tif_dir.td_samplesperpixel;
	if (!TIFFFieldSet(tif, FIELD_STRIPOFFSETS)) {
#ifdef OJPEG_SUPPORT
		if ((tif->tif_dir.td_compression==COMPRESSION_OJPEG) &&
		    (isTiled(tif)==0) &&
		    (tif->tif_dir.td_nstrips==1)) {
			/*
			 * XXX: OJPEG hack.
			 * If a) compression is OJPEG, b) it's not a tiled TIFF,
			 * and c) the number of strips is 1,
			 * then we tolerate the absence of stripoffsets tag,
			 * because, presumably, all required data is in the
			 * JpegInterchangeFormat stream.
			 */
			TIFFSetFieldBit(tif, FIELD_STRIPOFFSETS);
		} else
#endif
        {
			MissingRequired(tif,
				isTiled(tif) ? "TileOffsets" : "StripOffsets");
			goto bad;
		}
	}
	/*
	 * Second pass: extract other information.
	 */
	for (di=0, dp=dir; di<dircount; di++, dp++)
	{
		switch (dp->tdir_tag)
		{
			case IGNORE:
				break;
			case TIFFTAG_MINSAMPLEVALUE:
			case TIFFTAG_MAXSAMPLEVALUE:
			case TIFFTAG_BITSPERSAMPLE:
			case TIFFTAG_DATATYPE:
			case TIFFTAG_SAMPLEFORMAT:
				/*
				 * The MinSampleValue, MaxSampleValue, BitsPerSample
				 * DataType and SampleFormat tags are supposed to be
				 * written as one value/sample, but some vendors
				 * incorrectly write one value only -- so we accept
				 * that as well (yuck). Other vendors write correct
				 * value for NumberOfSamples, but incorrect one for
				 * BitsPerSample and friends, and we will read this
				 * too.
				 */
				{
					uint16 value;
					enum TIFFReadDirEntryErr err;
					err=TIFFReadDirEntryShort(tif,dp,&value);
					if (err==TIFFReadDirEntryErrCount)
						err=TIFFReadDirEntryPersampleShort(tif,dp,&value);
					if (err!=TIFFReadDirEntryErrOk)
					{
						fip = TIFFFieldWithTag(tif,dp->tdir_tag);
						TIFFReadDirEntryOutputErr(tif,err,module,fip ? fip->field_name : "unknown tagname",0);
						goto bad;
					}
					if (!TIFFSetField(tif,dp->tdir_tag,value))
						goto bad;
                    if( dp->tdir_tag == TIFFTAG_BITSPERSAMPLE )
                        bitspersample_read = TRUE;
				}
				break;
			case TIFFTAG_SMINSAMPLEVALUE:
			case TIFFTAG_SMAXSAMPLEVALUE:
				{

					double *data = NULL;
					enum TIFFReadDirEntryErr err;
					uint32 saved_flags;
					int m;
					if (dp->tdir_count != (uint64)tif->tif_dir.td_samplesperpixel)
						err = TIFFReadDirEntryErrCount;
					else
						err = TIFFReadDirEntryDoubleArray(tif, dp, &data);
					if (err!=TIFFReadDirEntryErrOk)
					{
						fip = TIFFFieldWithTag(tif,dp->tdir_tag);
						TIFFReadDirEntryOutputErr(tif,err,module,fip ? fip->field_name : "unknown tagname",0);
						goto bad;
					}
					saved_flags = tif->tif_flags;
					tif->tif_flags |= TIFF_PERSAMPLE;
					m = TIFFSetField(tif,dp->tdir_tag,data);
					tif->tif_flags = saved_flags;
					_TIFFfree(data);
					if (!m)
						goto bad;
				}
				break;
			case TIFFTAG_STRIPOFFSETS:
			case TIFFTAG_TILEOFFSETS:
#if defined(DEFER_STRILE_LOAD)
                                _TIFFmemcpy( &(tif->tif_dir.td_stripoffset_entry),
                                             dp, sizeof(TIFFDirEntry) );
#else                          
                                if( tif->tif_dir.td_stripoffset != NULL )
                                {
                                    TIFFErrorExt(tif->tif_clientdata, module,
                                        "tif->tif_dir.td_stripoffset is "
                                        "already allocated. Likely duplicated "
                                        "StripOffsets/TileOffsets tag");
                                    goto bad;
                                }
				if (!TIFFFetchStripThing(tif,dp,tif->tif_dir.td_nstrips,&tif->tif_dir.td_stripoffset))  
					goto bad;
#endif                                
				break;
			case TIFFTAG_STRIPBYTECOUNTS:
			case TIFFTAG_TILEBYTECOUNTS:
#if defined(DEFER_STRILE_LOAD)
                                _TIFFmemcpy( &(tif->tif_dir.td_stripbytecount_entry),
                                             dp, sizeof(TIFFDirEntry) );
#else                          
                                if( tif->tif_dir.td_stripbytecount != NULL )
                                {
                                    TIFFErrorExt(tif->tif_clientdata, module,
                                        "tif->tif_dir.td_stripbytecount is "
                                        "already allocated. Likely duplicated "
                                        "StripByteCounts/TileByteCounts tag");
                                    goto bad;
                                }
                                if (!TIFFFetchStripThing(tif,dp,tif->tif_dir.td_nstrips,&tif->tif_dir.td_stripbytecount))  
					goto bad;
#endif                                
				break;
			case TIFFTAG_COLORMAP:
			case TIFFTAG_TRANSFERFUNCTION:
				{
					enum TIFFReadDirEntryErr err;
					uint32 countpersample;
					uint32 countrequired;
					uint32 incrementpersample;
					uint16* value=NULL;
                    /* It would be dangerous to instantiate those tag values */
                    /* since if td_bitspersample has not yet been read (due to */
                    /* unordered tags), it could be read afterwards with a */
                    /* values greater than the default one (1), which may cause */
                    /* crashes in user code */
                    if( !bitspersample_read )
                    {
                        fip = TIFFFieldWithTag(tif,dp->tdir_tag);
                        TIFFWarningExt(tif->tif_clientdata,module,
                                       "Ignoring %s since BitsPerSample tag not found",
                                       fip ? fip->field_name : "unknown tagname");
                        continue;
                    }
					/* ColorMap or TransferFunction for high bit */
					/* depths do not make much sense and could be */
					/* used as a denial of service vector */
					if (tif->tif_dir.td_bitspersample > 24)
					{
					    fip = TIFFFieldWithTag(tif,dp->tdir_tag);
					    TIFFWarningExt(tif->tif_clientdata,module,
						"Ignoring %s because BitsPerSample=%d>24",
						fip ? fip->field_name : "unknown tagname",
						tif->tif_dir.td_bitspersample);
					    continue;
					}
					countpersample=(1U<<tif->tif_dir.td_bitspersample);
					if ((dp->tdir_tag==TIFFTAG_TRANSFERFUNCTION)&&(dp->tdir_count==(uint64)countpersample))
					{
						countrequired=countpersample;
						incrementpersample=0;
					}
					else
					{
						countrequired=3*countpersample;
						incrementpersample=countpersample;
					}
					if (dp->tdir_count!=(uint64)countrequired)
						err=TIFFReadDirEntryErrCount;
					else
						err=TIFFReadDirEntryShortArray(tif,dp,&value);
					if (err!=TIFFReadDirEntryErrOk)
                    {
						fip = TIFFFieldWithTag(tif,dp->tdir_tag);
						TIFFReadDirEntryOutputErr(tif,err,module,fip ? fip->field_name : "unknown tagname",1);
                    }
					else
					{
						TIFFSetField(tif,dp->tdir_tag,value,value+incrementpersample,value+2*incrementpersample);
						_TIFFfree(value);
					}
				}
				break;
/* BEGIN REV 4.0 COMPATIBILITY */
			case TIFFTAG_OSUBFILETYPE:
				{
					uint16 valueo;
					uint32 value;
					if (TIFFReadDirEntryShort(tif,dp,&valueo)==TIFFReadDirEntryErrOk)
					{
						switch (valueo)
						{
							case OFILETYPE_REDUCEDIMAGE: value=FILETYPE_REDUCEDIMAGE; break;
							case OFILETYPE_PAGE: value=FILETYPE_PAGE; break;
							default: value=0; break;
						}
						if (value!=0)
							TIFFSetField(tif,TIFFTAG_SUBFILETYPE,value);
					}
				}
				break;
/* END REV 4.0 COMPATIBILITY */
			default:
				(void) TIFFFetchNormalTag(tif, dp, TRUE);
				break;
		}
	}
	/*
	 * OJPEG hack:
	 * - If a) compression is OJPEG, and b) photometric tag is missing,
	 * then we consistently find that photometric should be YCbCr
	 * - If a) compression is OJPEG, and b) photometric tag says it's RGB,
	 * then we consistently find that the buggy implementation of the
	 * buggy compression scheme matches photometric YCbCr instead.
	 * - If a) compression is OJPEG, and b) bitspersample tag is missing,
	 * then we consistently find bitspersample should be 8.
	 * - If a) compression is OJPEG, b) samplesperpixel tag is missing,
	 * and c) photometric is RGB or YCbCr, then we consistently find
	 * samplesperpixel should be 3
	 * - If a) compression is OJPEG, b) samplesperpixel tag is missing,
	 * and c) photometric is MINISWHITE or MINISBLACK, then we consistently
	 * find samplesperpixel should be 3
	 */
	if (tif->tif_dir.td_compression==COMPRESSION_OJPEG)
	{
		if (!TIFFFieldSet(tif,FIELD_PHOTOMETRIC))
		{
			TIFFWarningExt(tif->tif_clientdata, module,
			    "Photometric tag is missing, assuming data is YCbCr");
			if (!TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_YCBCR))
				goto bad;
		}
		else if (tif->tif_dir.td_photometric==PHOTOMETRIC_RGB)
		{
			tif->tif_dir.td_photometric=PHOTOMETRIC_YCBCR;
			TIFFWarningExt(tif->tif_clientdata, module,
			    "Photometric tag value assumed incorrect, "
			    "assuming data is YCbCr instead of RGB");
		}
		if (!TIFFFieldSet(tif,FIELD_BITSPERSAMPLE))
		{
			TIFFWarningExt(tif->tif_clientdata,module,
			    "BitsPerSample tag is missing, assuming 8 bits per sample");
			if (!TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,8))
				goto bad;
		}
		if (!TIFFFieldSet(tif,FIELD_SAMPLESPERPIXEL))
		{
			if (tif->tif_dir.td_photometric==PHOTOMETRIC_RGB)
			{
				TIFFWarningExt(tif->tif_clientdata,module,
				    "SamplesPerPixel tag is missing, "
				    "assuming correct SamplesPerPixel value is 3");
				if (!TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,3))
					goto bad;
			}
			if (tif->tif_dir.td_photometric==PHOTOMETRIC_YCBCR)
			{
				TIFFWarningExt(tif->tif_clientdata,module,
				    "SamplesPerPixel tag is missing, "
				    "applying correct SamplesPerPixel value of 3");
				if (!TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,3))
					goto bad;
			}
			else if ((tif->tif_dir.td_photometric==PHOTOMETRIC_MINISWHITE)
				 || (tif->tif_dir.td_photometric==PHOTOMETRIC_MINISBLACK))
			{
				/*
				 * SamplesPerPixel tag is missing, but is not required
				 * by spec.  Assume correct SamplesPerPixel value of 1.
				 */
				if (!TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,1))
					goto bad;
			}
		}
	}

	/*
	 * Make sure all non-color channels are extrasamples.
	 * If it's not the case, define them as such.
	 */
        color_channels = _TIFFGetMaxColorChannels(tif->tif_dir.td_photometric);
        if (color_channels && tif->tif_dir.td_samplesperpixel - tif->tif_dir.td_extrasamples > color_channels) {
                uint16 old_extrasamples;
                uint16 *new_sampleinfo;

                TIFFWarningExt(tif->tif_clientdata,module, "Sum of Photometric type-related "
                    "color channels and ExtraSamples doesn't match SamplesPerPixel. "
                    "Defining non-color channels as ExtraSamples.");

                old_extrasamples = tif->tif_dir.td_extrasamples;
                tif->tif_dir.td_extrasamples = (tif->tif_dir.td_samplesperpixel - color_channels);

                // sampleinfo should contain information relative to these new extra samples
                new_sampleinfo = (uint16*) _TIFFcalloc(tif->tif_dir.td_extrasamples, sizeof(uint16));
                if (!new_sampleinfo) {
                    TIFFErrorExt(tif->tif_clientdata, module, "Failed to allocate memory for "
                                "temporary new sampleinfo array (%d 16 bit elements)",
                                tif->tif_dir.td_extrasamples);
                    goto bad;
                }

                memcpy(new_sampleinfo, tif->tif_dir.td_sampleinfo, old_extrasamples * sizeof(uint16));
                _TIFFsetShortArray(&tif->tif_dir.td_sampleinfo, new_sampleinfo, tif->tif_dir.td_extrasamples);
                _TIFFfree(new_sampleinfo);
        }

	/*
	 * Verify Palette image has a Colormap.
	 */
	if (tif->tif_dir.td_photometric == PHOTOMETRIC_PALETTE &&
	    !TIFFFieldSet(tif, FIELD_COLORMAP)) {
		if ( tif->tif_dir.td_bitspersample>=8 && tif->tif_dir.td_samplesperpixel==3)
			tif->tif_dir.td_photometric = PHOTOMETRIC_RGB;
		else if (tif->tif_dir.td_bitspersample>=8)
			tif->tif_dir.td_photometric = PHOTOMETRIC_MINISBLACK;
		else {
			MissingRequired(tif, "Colormap");
			goto bad;
		}
	}
	/*
	 * OJPEG hack:
	 * We do no further messing with strip/tile offsets/bytecounts in OJPEG
	 * TIFFs
	 */
	if (tif->tif_dir.td_compression!=COMPRESSION_OJPEG)
	{
		/*
		 * Attempt to deal with a missing StripByteCounts tag.
		 */
		if (!TIFFFieldSet(tif, FIELD_STRIPBYTECOUNTS)) {
			/*
			 * Some manufacturers violate the spec by not giving
			 * the size of the strips.  In this case, assume there
			 * is one uncompressed strip of data.
			 */
			if ((tif->tif_dir.td_planarconfig == PLANARCONFIG_CONTIG &&
			    tif->tif_dir.td_nstrips > 1) ||
			    (tif->tif_dir.td_planarconfig == PLANARCONFIG_SEPARATE &&
			     tif->tif_dir.td_nstrips != (uint32)tif->tif_dir.td_samplesperpixel)) {
			    MissingRequired(tif, "StripByteCounts");
			    goto bad;
			}
			TIFFWarningExt(tif->tif_clientdata, module,
				"TIFF directory is missing required "
				"\"StripByteCounts\" field, calculating from imagelength");
			if (EstimateStripByteCounts(tif, dir, dircount) < 0)
			    goto bad;
		/*
		 * Assume we have wrong StripByteCount value (in case
		 * of single strip) in following cases:
		 *   - it is equal to zero along with StripOffset;
		 *   - it is larger than file itself (in case of uncompressed
		 *     image);
		 *   - it is smaller than the size of the bytes per row
		 *     multiplied on the number of rows.  The last case should
		 *     not be checked in the case of writing new image,
		 *     because we may do not know the exact strip size
		 *     until the whole image will be written and directory
		 *     dumped out.
		 */
		#define	BYTECOUNTLOOKSBAD \
		    ( (tif->tif_dir.td_stripbytecount[0] == 0 && tif->tif_dir.td_stripoffset[0] != 0) || \
		      (tif->tif_dir.td_compression == COMPRESSION_NONE && \
		       (tif->tif_dir.td_stripoffset[0] <= TIFFGetFileSize(tif) && \
		        tif->tif_dir.td_stripbytecount[0] > TIFFGetFileSize(tif) - tif->tif_dir.td_stripoffset[0])) || \
		      (tif->tif_mode == O_RDONLY && \
		       tif->tif_dir.td_compression == COMPRESSION_NONE && \
		       tif->tif_dir.td_stripbytecount[0] < TIFFScanlineSize64(tif) * tif->tif_dir.td_imagelength) )

		} else if (tif->tif_dir.td_nstrips == 1
                           && !(tif->tif_flags&TIFF_ISTILED)
                           && _TIFFFillStriles(tif)
			   && tif->tif_dir.td_stripoffset[0] != 0
			   && BYTECOUNTLOOKSBAD) {
			/*
			 * XXX: Plexus (and others) sometimes give a value of
			 * zero for a tag when they don't know what the
			 * correct value is!  Try and handle the simple case
			 * of estimating the size of a one strip image.
			 */
			TIFFWarningExt(tif->tif_clientdata, module,
			    "Bogus \"StripByteCounts\" field, ignoring and calculating from imagelength");
			if(EstimateStripByteCounts(tif, dir, dircount) < 0)
			    goto bad;

#if !defined(DEFER_STRILE_LOAD)
		} else if (tif->tif_dir.td_planarconfig == PLANARCONFIG_CONTIG
			   && tif->tif_dir.td_nstrips > 2
			   && tif->tif_dir.td_compression == COMPRESSION_NONE
			   && tif->tif_dir.td_stripbytecount[0] != tif->tif_dir.td_stripbytecount[1]
			   && tif->tif_dir.td_stripbytecount[0] != 0
			   && tif->tif_dir.td_stripbytecount[1] != 0 ) {
			/*
			 * XXX: Some vendors fill StripByteCount array with
			 * absolutely wrong values (it can be equal to
			 * StripOffset array, for example). Catch this case
			 * here.
                         *
                         * We avoid this check if deferring strile loading
                         * as it would always force us to load the strip/tile
                         * information.
			 */
			TIFFWarningExt(tif->tif_clientdata, module,
			    "Wrong \"StripByteCounts\" field, ignoring and calculating from imagelength");
			if (EstimateStripByteCounts(tif, dir, dircount) < 0)
			    goto bad;
#endif /* !defined(DEFER_STRILE_LOAD) */                        
		}
	}
	if (dir)
	{
		_TIFFfree(dir);
		dir=NULL;
	}
	if (!TIFFFieldSet(tif, FIELD_MAXSAMPLEVALUE))
	{
		if (tif->tif_dir.td_bitspersample>=16)
			tif->tif_dir.td_maxsamplevalue=0xFFFF;
		else
			tif->tif_dir.td_maxsamplevalue = (uint16)((1L<<tif->tif_dir.td_bitspersample)-1);
	}
	/*
	 * XXX: We can optimize checking for the strip bounds using the sorted
	 * bytecounts array. See also comments for TIFFAppendToStrip()
	 * function in tif_write.c.
	 */
#if !defined(DEFER_STRILE_LOAD)        
	if (tif->tif_dir.td_nstrips > 1) {
		uint32 strip;

		tif->tif_dir.td_stripbytecountsorted = 1;
		for (strip = 1; strip < tif->tif_dir.td_nstrips; strip++) {
			if (tif->tif_dir.td_stripoffset[strip - 1] >
			    tif->tif_dir.td_stripoffset[strip]) {
				tif->tif_dir.td_stripbytecountsorted = 0;
				break;
			}
		}
	}
#endif /* !defined(DEFER_STRILE_LOAD) */
        
	/*
	 * An opportunity for compression mode dependent tag fixup
	 */
	(*tif->tif_fixuptags)(tif);

	/*
	 * Some manufacturers make life difficult by writing
	 * large amounts of uncompressed data as a single strip.
	 * This is contrary to the recommendations of the spec.
	 * The following makes an attempt at breaking such images
	 * into strips closer to the recommended 8k bytes.  A
	 * side effect, however, is that the RowsPerStrip tag
	 * value may be changed.
	 */
	if ((tif->tif_dir.td_planarconfig==PLANARCONFIG_CONTIG)&&
	    (tif->tif_dir.td_nstrips==1)&&
	    (tif->tif_dir.td_compression==COMPRESSION_NONE)&&  
	    ((tif->tif_flags&(TIFF_STRIPCHOP|TIFF_ISTILED))==TIFF_STRIPCHOP))
    {
        if ( !_TIFFFillStriles(tif) || !tif->tif_dir.td_stripbytecount )
            return 0;
		ChopUpSingleUncompressedStrip(tif);
    }

        /*
         * Clear the dirty directory flag. 
         */
	tif->tif_flags &= ~TIFF_DIRTYDIRECT;
	tif->tif_flags &= ~TIFF_DIRTYSTRIP;

	/*
	 * Reinitialize i/o since we are starting on a new directory.
	 */
	tif->tif_row = (uint32) -1;
	tif->tif_curstrip = (uint32) -1;
	tif->tif_col = (uint32) -1;
	tif->tif_curtile = (uint32) -1;
	tif->tif_tilesize = (tmsize_t) -1;

	tif->tif_scanlinesize = TIFFScanlineSize(tif);
	if (!tif->tif_scanlinesize) {
		TIFFErrorExt(tif->tif_clientdata, module,
		    "Cannot handle zero scanline size");
		return (0);
	}

	if (isTiled(tif)) {
		tif->tif_tilesize = TIFFTileSize(tif);
		if (!tif->tif_tilesize) {
			TIFFErrorExt(tif->tif_clientdata, module,
			     "Cannot handle zero tile size");
			return (0);
		}
	} else {
		if (!TIFFStripSize(tif)) {
			TIFFErrorExt(tif->tif_clientdata, module,
			    "Cannot handle zero strip size");
			return (0);
		}
	}
	return (1);
bad:
	if (dir)
		_TIFFfree(dir);
	return (0);
}