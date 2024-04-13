TIFFCheckDirOffset(TIFF* tif, uint64 diroff)
{
	uint16 n;

	if (diroff == 0)			/* no more directories */
		return 0;
	if (tif->tif_dirnumber == 65535) {
	    TIFFErrorExt(tif->tif_clientdata, "TIFFCheckDirOffset",
			 "Cannot handle more than 65535 TIFF directories");
	    return 0;
	}

	for (n = 0; n < tif->tif_dirnumber && tif->tif_dirlist; n++) {
		if (tif->tif_dirlist[n] == diroff)
			return 0;
	}

	tif->tif_dirnumber++;

	if (tif->tif_dirlist == NULL || tif->tif_dirnumber > tif->tif_dirlistsize) {
		uint64* new_dirlist;

		/*
		 * XXX: Reduce memory allocation granularity of the dirlist
		 * array.
		 */
		new_dirlist = (uint64*)_TIFFCheckRealloc(tif, tif->tif_dirlist,
		    tif->tif_dirnumber, 2 * sizeof(uint64), "for IFD list");
		if (!new_dirlist)
			return 0;
		if( tif->tif_dirnumber >= 32768 )
		    tif->tif_dirlistsize = 65535;
		else
		    tif->tif_dirlistsize = 2 * tif->tif_dirnumber;
		tif->tif_dirlist = new_dirlist;
	}

	tif->tif_dirlist[tif->tif_dirnumber - 1] = diroff;

	return 1;
}