static void TIFFReadDirEntryCheckedSlong(TIFF* tif, TIFFDirEntry* direntry, int32* value)
{
	*value=*(int32*)(&direntry->tdir_offset);
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong((uint32*)value);
}