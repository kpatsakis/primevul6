_TIFFReadTileAndAllocBuffer(TIFF* tif,
                            void **buf, tmsize_t bufsizetoalloc,
                            uint32 x, uint32 y, uint32 z, uint16 s)
{
    if (!TIFFCheckRead(tif, 1) || !TIFFCheckTile(tif, x, y, z, s))
            return ((tmsize_t)(-1));
    return (_TIFFReadEncodedTileAndAllocBuffer(tif,
                                               TIFFComputeTile(tif, x, y, z, s),
                                               buf, bufsizetoalloc,
                                               (tmsize_t)(-1)));
}