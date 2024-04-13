static enum TIFFReadDirEntryErr TIFFReadDirEntryLong8Array(TIFF* tif, TIFFDirEntry* direntry, uint64** value)
{
    return TIFFReadDirEntryLong8ArrayWithLimit(tif, direntry, value, ~((uint64)0));
}