TIFFReadDirectoryCheckOrder(TIFF* tif, TIFFDirEntry* dir, uint16 dircount)
{
	static const char module[] = "TIFFReadDirectoryCheckOrder";
	uint16 m;
	uint16 n;
	TIFFDirEntry* o;
	m=0;
	for (n=0, o=dir; n<dircount; n++, o++)
	{
		if (o->tdir_tag<m)
		{
			TIFFWarningExt(tif->tif_clientdata,module,
			    "Invalid TIFF directory; tags are not sorted in ascending order");
			break;
		}
		m=o->tdir_tag+1;
	}
}