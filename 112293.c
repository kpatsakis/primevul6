TIFFStartStrip(TIFF* tif, uint32 strip)
{
	TIFFDirectory *td = &tif->tif_dir;

        if (!_TIFFFillStriles( tif ) || !tif->tif_dir.td_stripbytecount)
            return 0;

	if ((tif->tif_flags & TIFF_CODERSETUP) == 0) {
		if (!(*tif->tif_setupdecode)(tif))
			return (0);
		tif->tif_flags |= TIFF_CODERSETUP;
	}
	tif->tif_curstrip = strip;
	tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
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
			tif->tif_rawcc = (tmsize_t)td->td_stripbytecount[strip];
	}
	return ((*tif->tif_predecode)(tif,
			(uint16)(strip / td->td_stripsperimage)));
}