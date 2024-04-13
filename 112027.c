TIFFWriteDirectoryTagCheckedLong8(TIFF* tif, uint32* ndir, TIFFDirEntry* dir, uint16 tag, uint64 value)
{
	uint64 m;
	assert(sizeof(uint64)==8);
	if( !(tif->tif_flags&TIFF_BIGTIFF) ) {
		TIFFErrorExt(tif->tif_clientdata,"TIFFWriteDirectoryTagCheckedLong8","LONG8 not allowed for ClassicTIFF");
		return(0);
	}
	m=value;
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong8(&m);
	return(TIFFWriteDirectoryTagData(tif,ndir,dir,tag,TIFF_LONG8,1,8,&m));
}