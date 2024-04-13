TIFFFetchStripThing(TIFF* tif, TIFFDirEntry* dir, uint32 nstrips, uint64** lpp)
{
	static const char module[] = "TIFFFetchStripThing";
	enum TIFFReadDirEntryErr err;
	uint64* data;
	err=TIFFReadDirEntryLong8ArrayWithLimit(tif,dir,&data,nstrips);
	if (err!=TIFFReadDirEntryErrOk)
	{
		const TIFFField* fip = TIFFFieldWithTag(tif,dir->tdir_tag); 
		TIFFReadDirEntryOutputErr(tif,err,module,fip ? fip->field_name : "unknown tagname",0);
		return(0);
	}
	if (dir->tdir_count<(uint64)nstrips)
	{
		uint64* resizeddata;
		const TIFFField* fip = TIFFFieldWithTag(tif,dir->tdir_tag);
		const char* pszMax = getenv("LIBTIFF_STRILE_ARRAY_MAX_RESIZE_COUNT");
		uint32 max_nstrips = 1000000;
		if( pszMax )
			max_nstrips = (uint32) atoi(pszMax);
		TIFFReadDirEntryOutputErr(tif,TIFFReadDirEntryErrCount,
		            module,
		            fip ? fip->field_name : "unknown tagname",
		            ( nstrips <= max_nstrips ) );

		if( nstrips > max_nstrips )
		{
			_TIFFfree(data);
			return(0);
		}

		resizeddata=(uint64*)_TIFFCheckMalloc(tif,nstrips,sizeof(uint64),"for strip array");
		if (resizeddata==0) {
			_TIFFfree(data);
			return(0);
		}
                _TIFFmemcpy(resizeddata,data,(uint32)dir->tdir_count*sizeof(uint64));
                _TIFFmemset(resizeddata+(uint32)dir->tdir_count,0,(nstrips-(uint32)dir->tdir_count)*sizeof(uint64));
		_TIFFfree(data);
		data=resizeddata;
	}
	*lpp=data;
	return(1);
}