EstimateStripByteCounts(TIFF* tif, TIFFDirEntry* dir, uint16 dircount)
{
	static const char module[] = "EstimateStripByteCounts";

	TIFFDirEntry *dp;
	TIFFDirectory *td = &tif->tif_dir;
	uint32 strip;

    /* Do not try to load stripbytecount as we will compute it */
        if( !_TIFFFillStrilesInternal( tif, 0 ) )
            return -1;

	if (td->td_stripbytecount)
		_TIFFfree(td->td_stripbytecount);
	td->td_stripbytecount = (uint64*)
	    _TIFFCheckMalloc(tif, td->td_nstrips, sizeof (uint64),
		"for \"StripByteCounts\" array");
        if( td->td_stripbytecount == NULL )
            return -1;

	if (td->td_compression != COMPRESSION_NONE) {
		uint64 space;
		uint64 filesize;
		uint16 n;
		filesize = TIFFGetFileSize(tif);
		if (!(tif->tif_flags&TIFF_BIGTIFF))
			space=sizeof(TIFFHeaderClassic)+2+dircount*12+4;
		else
			space=sizeof(TIFFHeaderBig)+8+dircount*20+8;
		/* calculate amount of space used by indirect values */
		for (dp = dir, n = dircount; n > 0; n--, dp++)
		{
			uint32 typewidth;
			uint64 datasize;
			typewidth = TIFFDataWidth((TIFFDataType) dp->tdir_type);
			if (typewidth == 0) {
				TIFFErrorExt(tif->tif_clientdata, module,
				    "Cannot determine size of unknown tag type %d",
				    dp->tdir_type);
				return -1;
			}
			datasize=(uint64)typewidth*dp->tdir_count;
			if (!(tif->tif_flags&TIFF_BIGTIFF))
			{
				if (datasize<=4)
					datasize=0;
			}
			else
			{
				if (datasize<=8)
					datasize=0;
			}
			space+=datasize;
		}
		if( filesize < space )
                    /* we should perhaps return in error ? */
                    space = filesize;
                else
                    space = filesize - space;
		if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
			space /= td->td_samplesperpixel;
		for (strip = 0; strip < td->td_nstrips; strip++)
			td->td_stripbytecount[strip] = space;
		/*
		 * This gross hack handles the case were the offset to
		 * the last strip is past the place where we think the strip
		 * should begin.  Since a strip of data must be contiguous,
		 * it's safe to assume that we've overestimated the amount
		 * of data in the strip and trim this number back accordingly.
		 */
		strip--;
		if (td->td_stripoffset[strip]+td->td_stripbytecount[strip] > filesize)
			td->td_stripbytecount[strip] = filesize - td->td_stripoffset[strip];
	} else if (isTiled(tif)) {
		uint64 bytespertile = TIFFTileSize64(tif);

		for (strip = 0; strip < td->td_nstrips; strip++)
		    td->td_stripbytecount[strip] = bytespertile;
	} else {
		uint64 rowbytes = TIFFScanlineSize64(tif);
		uint32 rowsperstrip = td->td_imagelength/td->td_stripsperimage;
		for (strip = 0; strip < td->td_nstrips; strip++)
			td->td_stripbytecount[strip] = rowbytes * rowsperstrip;
	}
	TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
	if (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP))
		td->td_rowsperstrip = td->td_imagelength;
	return 1;
}