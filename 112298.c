_TIFFSwab24BitData(TIFF* tif, uint8* buf, tmsize_t cc)
{
    (void) tif;
    assert((cc % 3) == 0);
    TIFFSwabArrayOfTriples((uint8*) buf, cc/3);
}