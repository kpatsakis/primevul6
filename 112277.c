_TIFFSwab32BitData(TIFF* tif, uint8* buf, tmsize_t cc)
{
    (void) tif;
    assert((cc & 3) == 0);
    TIFFSwabArrayOfLong((uint32*) buf, cc/4);
}