static void TIFFReadDirEntryCheckedLong(TIFF* tif, TIFFDirEntry* direntry, uint32* value)
{
	*value=*(uint32*)(&direntry->tdir_offset);
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong(value);
}