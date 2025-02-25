CheckDirCount(TIFF* tif, TIFFDirEntry* dir, uint32 count)
{
	if ((uint64)count > dir->tdir_count) {
		const TIFFField* fip = TIFFFieldWithTag(tif, dir->tdir_tag);
		TIFFWarningExt(tif->tif_clientdata, tif->tif_name,
	"incorrect count for field \"%s\" (" TIFF_UINT64_FORMAT ", expecting %u); tag ignored",
		    fip ? fip->field_name : "unknown tagname",
		    dir->tdir_count, count);
		return (0);
	} else if ((uint64)count < dir->tdir_count) {
		const TIFFField* fip = TIFFFieldWithTag(tif, dir->tdir_tag);
		TIFFWarningExt(tif->tif_clientdata, tif->tif_name,
	"incorrect count for field \"%s\" (" TIFF_UINT64_FORMAT ", expecting %u); tag trimmed",
		    fip ? fip->field_name : "unknown tagname",
		    dir->tdir_count, count);
		dir->tdir_count = count;
		return (1);
	}
	return (1);
}