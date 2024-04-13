static void TIFFReadDirEntryCheckedSbyte(TIFF* tif, TIFFDirEntry* direntry, int8* value)
{
	(void) tif;
	*value=*(int8*)(&direntry->tdir_offset);
}