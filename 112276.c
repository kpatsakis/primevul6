TIFFReadRawStrip(TIFF* tif, uint32 strip, void* buf, tmsize_t size)
{
	static const char module[] = "TIFFReadRawStrip";
	TIFFDirectory *td = &tif->tif_dir;
	uint64 bytecount;
	tmsize_t bytecountm;

	if (!TIFFCheckRead(tif, 0))
		return ((tmsize_t)(-1));
	if (strip >= td->td_nstrips) {
		TIFFErrorExt(tif->tif_clientdata, module,
		     "%lu: Strip out of range, max %lu",
		     (unsigned long) strip,
		     (unsigned long) td->td_nstrips);
		return ((tmsize_t)(-1));
	}
	if (tif->tif_flags&TIFF_NOREADRAW)
	{
		TIFFErrorExt(tif->tif_clientdata, module,
		    "Compression scheme does not support access to raw uncompressed data");
		return ((tmsize_t)(-1));
	}
	bytecount = td->td_stripbytecount[strip];
	if ((int64)bytecount <= 0) {
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
		TIFFErrorExt(tif->tif_clientdata, module,
			     "%I64u: Invalid strip byte count, strip %lu",
			     (unsigned __int64) bytecount,
			     (unsigned long) strip);
#else
		TIFFErrorExt(tif->tif_clientdata, module,
			     "%llu: Invalid strip byte count, strip %lu",
			     (unsigned long long) bytecount,
			     (unsigned long) strip);
#endif
		return ((tmsize_t)(-1));
	}
	bytecountm = (tmsize_t)bytecount;
	if ((uint64)bytecountm!=bytecount) {
		TIFFErrorExt(tif->tif_clientdata, module, "Integer overflow");
		return ((tmsize_t)(-1));
	}
	if (size != (tmsize_t)(-1) && size < bytecountm)
		bytecountm = size;
	return (TIFFReadRawStrip1(tif, strip, buf, bytecountm, module));
}