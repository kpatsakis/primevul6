TIFFStartTile(TIFF* tif, uint32 tile)
{
        static const char module[] = "TIFFStartTile";
	TIFFDirectory *td = &tif->tif_dir;
        uint32 howmany32;

        if (!_TIFFFillStriles( tif ) || !tif->tif_dir.td_stripbytecount)
                return 0;

	if ((tif->tif_flags & TIFF_CODERSETUP) == 0) {
		if (!(*tif->tif_setupdecode)(tif))
			return (0);
		tif->tif_flags |= TIFF_CODERSETUP;
	}
	tif->tif_curtile = tile;
        howmany32=TIFFhowmany_32(td->td_imagewidth, td->td_tilewidth);
        if (howmany32 == 0) {
                 TIFFErrorExt(tif->tif_clientdata,module,"Zero tiles");
                return 0;
        }
	tif->tif_row = (tile % howmany32) * td->td_tilelength;
        howmany32=TIFFhowmany_32(td->td_imagelength, td->td_tilelength);
        if (howmany32 == 0) {
                TIFFErrorExt(tif->tif_clientdata,module,"Zero tiles");
                return 0;
        }
	tif->tif_col = (tile % howmany32) * td->td_tilewidth;
        tif->tif_flags &= ~TIFF_BUF4WRITE;
	if (tif->tif_flags&TIFF_NOREADRAW)
	{
		tif->tif_rawcp = NULL;
		tif->tif_rawcc = 0;
	}
	else
	{
		tif->tif_rawcp = tif->tif_rawdata;
		if( tif->tif_rawdataloaded > 0 )
			tif->tif_rawcc = tif->tif_rawdataloaded;
		else
			tif->tif_rawcc = (tmsize_t)td->td_stripbytecount[tile];
	}
	return ((*tif->tif_predecode)(tif,
			(uint16)(tile/td->td_stripsperimage)));
}