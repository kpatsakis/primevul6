TIFFReadDirEntryData(TIFF* tif, uint64 offset, tmsize_t size, void* dest)
{
	assert(size>0);
	if (!isMapped(tif)) {
		if (!SeekOK(tif,offset))
			return(TIFFReadDirEntryErrIo);
		if (!ReadOK(tif,dest,size))
			return(TIFFReadDirEntryErrIo);
	} else {
		size_t ma,mb;
		ma=(size_t)offset;
		mb=ma+size;
		if (((uint64)ma!=offset)
		    || (mb < ma)
		    || (mb - ma != (size_t) size)
		    || (mb < (size_t)size)
		    || (mb > (size_t)tif->tif_size)
		    )
			return(TIFFReadDirEntryErrIo);
		_TIFFmemcpy(dest,tif->tif_base+ma,size);
	}
	return(TIFFReadDirEntryErrOk);
}