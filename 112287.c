TIFFReadTile(TIFF* tif, void* buf, uint32 x, uint32 y, uint32 z, uint16 s)
{
	if (!TIFFCheckRead(tif, 1) || !TIFFCheckTile(tif, x, y, z, s))
		return ((tmsize_t)(-1));
	return (TIFFReadEncodedTile(tif,
	    TIFFComputeTile(tif, x, y, z, s), buf, (tmsize_t)(-1)));
}