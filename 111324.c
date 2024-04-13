MissingRequired(TIFF* tif, const char* tagname)
{
	static const char module[] = "MissingRequired";

	TIFFErrorExt(tif->tif_clientdata, module,
	    "TIFF directory is missing required \"%s\" field",
	    tagname);
}