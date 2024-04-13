static enum TIFFReadDirEntryErr TIFFReadDirEntryArray(TIFF* tif, TIFFDirEntry* direntry, uint32* count, uint32 desttypesize, void** value)
{
    return TIFFReadDirEntryArrayWithLimit(tif, direntry, count,
                                          desttypesize, value, ~((uint64)0));
}