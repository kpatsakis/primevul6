_TIFFReadEncodedTileAndAllocBuffer(TIFF* tif, uint32 tile,
                                    void **buf, tmsize_t bufsizetoalloc,
                                    tmsize_t size_to_read)
{
    static const char module[] = "_TIFFReadEncodedTileAndAllocBuffer";
    TIFFDirectory *td = &tif->tif_dir;
    tmsize_t tilesize = tif->tif_tilesize;

    if( *buf != NULL )
    {
        return TIFFReadEncodedTile(tif, tile, *buf, size_to_read);
    }

    if (!TIFFCheckRead(tif, 1))
            return ((tmsize_t)(-1));
    if (tile >= td->td_nstrips) {
            TIFFErrorExt(tif->tif_clientdata, module,
                "%lu: Tile out of range, max %lu",
                (unsigned long) tile, (unsigned long) td->td_nstrips);
            return ((tmsize_t)(-1));
    }

    if (!TIFFFillTile(tif,tile))
            return((tmsize_t)(-1));

    *buf = _TIFFmalloc(bufsizetoalloc);
    if (*buf == NULL) {
            TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif),
                         "No space for tile buffer");
            return((tmsize_t)(-1));
    }
    _TIFFmemset(*buf, 0, bufsizetoalloc);

    if (size_to_read == (tmsize_t)(-1))
        size_to_read = tilesize;
    else if (size_to_read > tilesize)
        size_to_read = tilesize;
    if( (*tif->tif_decodetile)(tif,
        (uint8*) *buf, size_to_read, (uint16)(tile/td->td_stripsperimage))) {
        (*tif->tif_postdecode)(tif, (uint8*) *buf, size_to_read);
        return (size_to_read);
    } else
        return ((tmsize_t)(-1));
}