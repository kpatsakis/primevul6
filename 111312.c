ChopUpSingleUncompressedStrip(TIFF* tif)
{
	register TIFFDirectory *td = &tif->tif_dir;
	uint64 bytecount;
	uint64 offset;
	uint32 rowblock;
	uint64 rowblockbytes;
	uint64 stripbytes;
	uint32 strip;
	uint32 nstrips;
	uint32 rowsperstrip;
	uint64* newcounts;
	uint64* newoffsets;

	bytecount = td->td_stripbytecount[0];
        /* On a newly created file, just re-opened to be filled, we */
        /* don't want strip chop to trigger as it is going to cause issues */
        /* later ( StripOffsets and StripByteCounts improperly filled) . */
        if( bytecount == 0 && tif->tif_mode != O_RDONLY )
            return;
	offset = td->td_stripoffset[0];
	assert(td->td_planarconfig == PLANARCONFIG_CONTIG);
	if ((td->td_photometric == PHOTOMETRIC_YCBCR)&&
	    (!isUpSampled(tif)))
		rowblock = td->td_ycbcrsubsampling[1];
	else
		rowblock = 1;
	rowblockbytes = TIFFVTileSize64(tif, rowblock);
	/*
	 * Make the rows hold at least one scanline, but fill specified amount
	 * of data if possible.
	 */
	if (rowblockbytes > STRIP_SIZE_DEFAULT) {
		stripbytes = rowblockbytes;
		rowsperstrip = rowblock;
	} else if (rowblockbytes > 0 ) {
		uint32 rowblocksperstrip;
		rowblocksperstrip = (uint32) (STRIP_SIZE_DEFAULT / rowblockbytes);
		rowsperstrip = rowblocksperstrip * rowblock;
		stripbytes = rowblocksperstrip * rowblockbytes;
	}
	else
	    return;

	/*
	 * never increase the number of rows per strip
	 */
	if (rowsperstrip >= td->td_rowsperstrip)
		return;
        nstrips = TIFFhowmany_32(td->td_imagelength, rowsperstrip);
        if( nstrips == 0 )
            return;

        /* If we are going to allocate a lot of memory, make sure that the */
        /* file is as big as needed */
        if( tif->tif_mode == O_RDONLY &&
            nstrips > 1000000 &&
            (offset >= TIFFGetFileSize(tif) ||
             stripbytes > (TIFFGetFileSize(tif) - offset) / (nstrips - 1)) )
        {
            return;
        }

	newcounts = (uint64*) _TIFFCheckMalloc(tif, nstrips, sizeof (uint64),
				"for chopped \"StripByteCounts\" array");
	newoffsets = (uint64*) _TIFFCheckMalloc(tif, nstrips, sizeof (uint64),
				"for chopped \"StripOffsets\" array");
	if (newcounts == NULL || newoffsets == NULL) {
		/*
		 * Unable to allocate new strip information, give up and use
		 * the original one strip information.
		 */
		if (newcounts != NULL)
			_TIFFfree(newcounts);
		if (newoffsets != NULL)
			_TIFFfree(newoffsets);
		return;
	}
	/*
	 * Fill the strip information arrays with new bytecounts and offsets
	 * that reflect the broken-up format.
	 */
	for (strip = 0; strip < nstrips; strip++) {
		if (stripbytes > bytecount)
			stripbytes = bytecount;
		newcounts[strip] = stripbytes;
		newoffsets[strip] = stripbytes ? offset : 0;
		offset += stripbytes;
		bytecount -= stripbytes;
	}
	/*
	 * Replace old single strip info with multi-strip info.
	 */
	td->td_stripsperimage = td->td_nstrips = nstrips;
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

	_TIFFfree(td->td_stripbytecount);
	_TIFFfree(td->td_stripoffset);
	td->td_stripbytecount = newcounts;
	td->td_stripoffset = newoffsets;
	td->td_stripbytecountsorted = 1;
}