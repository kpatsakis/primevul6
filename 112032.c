TIFFWriteDirectoryTagCheckedSlong8(TIFF* tif, uint32* ndir, TIFFDirEntry* dir, uint16 tag, int64 value)
{
	int64 m;
	assert(sizeof(int64)==8);
	if( !(tif->tif_flags&TIFF_BIGTIFF) ) {
		TIFFErrorExt(tif->tif_clientdata,"TIFFWriteDirectoryTagCheckedSlong8","SLONG8 not allowed for ClassicTIFF");
		return(0);
	}
	m=value;
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong8((uint64*)(&m));
	return(TIFFWriteDirectoryTagData(tif,ndir,dir,tag,TIFF_SLONG8,1,8,&m));
}