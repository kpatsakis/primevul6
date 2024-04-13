TIFFReadRawTile(TIFF* tif, uint32 tile, void* buf, tmsize_t size)
{
	static const char module[] = "TIFFReadRawTile";
	TIFFDirectory *td = &tif->tif_dir;
	uint64 bytecount64;
	tmsize_t bytecountm;

	if (!TIFFCheckRead(tif, 1))
		return ((tmsize_t)(-1));
	if (tile >= td->td_nstrips) {
		TIFFErrorExt(tif->tif_clientdata, module,
		    "%lu: Tile out of range, max %lu",
		    (unsigned long) tile, (unsigned long) td->td_nstrips);
		return ((tmsize_t)(-1));
	}
	if (tif->tif_flags&TIFF_NOREADRAW)
	{
		TIFFErrorExt(tif->tif_clientdata, module,
		"Compression scheme does not support access to raw uncompressed data");
		return ((tmsize_t)(-1));
	}
	bytecount64 = td->td_stripbytecount[tile];
	if (size != (tmsize_t)(-1) && (uint64)size < bytecount64)
		bytecount64 = (uint64)size;
	bytecountm = (tmsize_t)bytecount64;
	if ((uint64)bytecountm!=bytecount64)
	{
		TIFFErrorExt(tif->tif_clientdata,module,"Integer overflow");
		return ((tmsize_t)(-1));
	}
	return (TIFFReadRawTile1(tif, tile, buf, bytecountm, module));
}