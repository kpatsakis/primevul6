TIFFReadRawStrip1(TIFF* tif, uint32 strip, void* buf, tmsize_t size,
    const char* module)
{
	TIFFDirectory *td = &tif->tif_dir;

    if (!_TIFFFillStriles( tif ))
        return ((tmsize_t)(-1));
        
	assert((tif->tif_flags&TIFF_NOREADRAW)==0);
	if (!isMapped(tif)) {
		tmsize_t cc;

		if (!SeekOK(tif, td->td_stripoffset[strip])) {
			TIFFErrorExt(tif->tif_clientdata, module,
			    "Seek error at scanline %lu, strip %lu",
			    (unsigned long) tif->tif_row, (unsigned long) strip);
			return ((tmsize_t)(-1));
		}
		cc = TIFFReadFile(tif, buf, size);
		if (cc != size) {
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			TIFFErrorExt(tif->tif_clientdata, module,
		"Read error at scanline %lu; got %I64u bytes, expected %I64u",
				     (unsigned long) tif->tif_row,
				     (unsigned __int64) cc,
				     (unsigned __int64) size);
#else
			TIFFErrorExt(tif->tif_clientdata, module,
		"Read error at scanline %lu; got %llu bytes, expected %llu",
				     (unsigned long) tif->tif_row,
				     (unsigned long long) cc,
				     (unsigned long long) size);
#endif
			return ((tmsize_t)(-1));
		}
	} else {
		tmsize_t ma = 0;
		tmsize_t n;
		if ((td->td_stripoffset[strip] > (uint64)TIFF_TMSIZE_T_MAX)||
                    ((ma=(tmsize_t)td->td_stripoffset[strip])>tif->tif_size))
                {
                    n=0;
                }
                else if( ma > TIFF_TMSIZE_T_MAX - size )
                {
                    n=0;
                }
                else
                {
                    tmsize_t mb=ma+size;
                    if (mb>tif->tif_size)
                            n=tif->tif_size-ma;
                    else
                            n=size;
                }
		if (n!=size) {
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			TIFFErrorExt(tif->tif_clientdata, module,
	"Read error at scanline %lu, strip %lu; got %I64u bytes, expected %I64u",
				     (unsigned long) tif->tif_row,
				     (unsigned long) strip,
				     (unsigned __int64) n,
				     (unsigned __int64) size);
#else
			TIFFErrorExt(tif->tif_clientdata, module,
	"Read error at scanline %lu, strip %lu; got %llu bytes, expected %llu",
				     (unsigned long) tif->tif_row,
				     (unsigned long) strip,
				     (unsigned long long) n,
				     (unsigned long long) size);
#endif
			return ((tmsize_t)(-1));
		}
		_TIFFmemcpy(buf, tif->tif_base + ma,
			    size);
	}
	return (size);
}