static int JBIGSetupEncode(TIFF* tif)
{
	if (TIFFNumberOfStrips(tif) != 1)
	{
		TIFFErrorExt(tif->tif_clientdata, "JBIG", "Multistrip images not supported in encoder");
		return 0;
	}

	return 1;
}