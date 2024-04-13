TIFFWriteDirectoryTagCheckedSlong8Array(TIFF* tif, uint32* ndir, TIFFDirEntry* dir, uint16 tag, uint32 count, int64* value)
{
	assert(count<0x20000000);
	assert(sizeof(int64)==8);
	if( !(tif->tif_flags&TIFF_BIGTIFF) ) {
		TIFFErrorExt(tif->tif_clientdata,"TIFFWriteDirectoryTagCheckedSlong8Array","SLONG8 not allowed for ClassicTIFF");
		return(0);
	}
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabArrayOfLong8((uint64*)value,count);
	return(TIFFWriteDirectoryTagData(tif,ndir,dir,tag,TIFF_SLONG8,count,count*8,value));
}