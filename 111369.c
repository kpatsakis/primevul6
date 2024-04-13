static void TIFFReadDirEntryCheckedSshort(TIFF* tif, TIFFDirEntry* direntry, int16* value)
{
	*value=*(int16*)(&direntry->tdir_offset);
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabShort((uint16*)value);
}