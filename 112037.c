TIFFWriteDirectoryTagSubifd(TIFF* tif, uint32* ndir, TIFFDirEntry* dir)
{
	static const char module[] = "TIFFWriteDirectoryTagSubifd";
	uint64 m;
	int n;
	if (tif->tif_dir.td_nsubifd==0)
		return(1);
	if (dir==NULL)
	{
		(*ndir)++;
		return(1);
	}
	m=tif->tif_dataoff;
	if (!(tif->tif_flags&TIFF_BIGTIFF))
	{
		uint32* o;
		uint64* pa;
		uint32* pb;
		uint16 p;
		o=_TIFFmalloc(tif->tif_dir.td_nsubifd*sizeof(uint32));
		if (o==NULL)
		{
			TIFFErrorExt(tif->tif_clientdata,module,"Out of memory");
			return(0);
		}
		pa=tif->tif_dir.td_subifd;
		pb=o;
		for (p=0; p < tif->tif_dir.td_nsubifd; p++)
		{
                        assert(pa != 0);

                        /* Could happen if an classicTIFF has a SubIFD of type LONG8 (which is illegal) */
                        if( *pa > 0xFFFFFFFFUL)
                        {
                            TIFFErrorExt(tif->tif_clientdata,module,"Illegal value for SubIFD tag");
                            _TIFFfree(o);
                            return(0);
                        }
			*pb++=(uint32)(*pa++);
		}
		n=TIFFWriteDirectoryTagCheckedIfdArray(tif,ndir,dir,TIFFTAG_SUBIFD,tif->tif_dir.td_nsubifd,o);
		_TIFFfree(o);
	}
	else
		n=TIFFWriteDirectoryTagCheckedIfd8Array(tif,ndir,dir,TIFFTAG_SUBIFD,tif->tif_dir.td_nsubifd,tif->tif_dir.td_subifd);
	if (!n)
		return(0);
	/*
	 * Total hack: if this directory includes a SubIFD
	 * tag then force the next <n> directories to be
	 * written as ``sub directories'' of this one.  This
	 * is used to write things like thumbnails and
	 * image masks that one wants to keep out of the
	 * normal directory linkage access mechanism.
	 */
	tif->tif_flags|=TIFF_INSUBIFD;
	tif->tif_nsubifd=tif->tif_dir.td_nsubifd;
	if (tif->tif_dir.td_nsubifd==1)
		tif->tif_subifdoff=0;
	else
		tif->tif_subifdoff=m;
	return(1);
}