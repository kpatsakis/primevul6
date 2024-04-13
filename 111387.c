TIFFReadEXIFDirectory(TIFF* tif, toff_t diroff)
{
	const TIFFFieldArray* exifFieldArray;
	exifFieldArray = _TIFFGetExifFields();
	return TIFFReadCustomDirectory(tif, diroff, exifFieldArray);  
}