int TIFFInitJBIG(TIFF* tif, int scheme)
{
	assert(scheme == COMPRESSION_JBIG);

	/*
	 * These flags are set so the JBIG Codec can control when to reverse
	 * bits and when not to and to allow the jbig decoder and bit reverser
	 * to write to memory when necessary.
	 */
	tif->tif_flags |= TIFF_NOBITREV;
	tif->tif_flags &= ~TIFF_MAPPED;

	/* Setup the function pointers for encode, decode, and cleanup. */
	tif->tif_setupdecode = JBIGSetupDecode;
	tif->tif_decodestrip = JBIGDecode;

	tif->tif_setupencode = JBIGSetupEncode;
	tif->tif_encodestrip = JBIGEncode;

	return 1;
}