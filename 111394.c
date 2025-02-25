static void TIFFReadDirEntryCheckedFloat(TIFF* tif, TIFFDirEntry* direntry, float* value)
{
         union
	 {
	   float  f;
	   uint32 i;
	 } float_union;
	assert(sizeof(float)==4);
	assert(sizeof(uint32)==4);
	assert(sizeof(float_union)==4);
	float_union.i=*(uint32*)(&direntry->tdir_offset);
	*value=float_union.f;
	if (tif->tif_flags&TIFF_SWAB)
		TIFFSwabLong((uint32*)value);
}