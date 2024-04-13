_TIFFReadEncodedStripAndAllocBuffer(TIFF* tif, uint32 strip,
                                    void **buf, tmsize_t bufsizetoalloc,
                                    tmsize_t size_to_read)
{
    tmsize_t this_stripsize;
    uint16 plane;

    if( *buf != NULL )
    {
        return TIFFReadEncodedStrip(tif, strip, *buf, size_to_read);
    }

    this_stripsize=TIFFReadEncodedStripGetStripSize(tif, strip, &plane);
    if (this_stripsize==((tmsize_t)(-1)))
            return((tmsize_t)(-1));

    if ((size_to_read!=(tmsize_t)(-1))&&(size_to_read<this_stripsize))
            this_stripsize=size_to_read;
    if (!TIFFFillStrip(tif,strip))
            return((tmsize_t)(-1));

    *buf = _TIFFmalloc(bufsizetoalloc);
    if (*buf == NULL) {
            TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif), "No space for strip buffer");
            return((tmsize_t)(-1));
    }
    _TIFFmemset(*buf, 0, bufsizetoalloc);

    if ((*tif->tif_decodestrip)(tif,*buf,this_stripsize,plane)<=0)
            return((tmsize_t)(-1));
    (*tif->tif_postdecode)(tif,*buf,this_stripsize);
    return(this_stripsize);


}