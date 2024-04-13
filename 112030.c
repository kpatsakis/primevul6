TIFFWriteDirectoryTagCheckedRational(TIFF* tif, uint32* ndir, TIFFDirEntry* dir, uint16 tag, double value)
{
        static const char module[] = "TIFFWriteDirectoryTagCheckedRational";
	uint32 m[2];
	assert(sizeof(uint32)==4);
        if( value < 0 )
        {
            TIFFErrorExt(tif->tif_clientdata,module,"Negative value is illegal");
            return 0;
        }
        else if( value != value )
        {
            TIFFErrorExt(tif->tif_clientdata,module,"Not-a-number value is illegal");
            return 0;
        }
	else if (value==0.0)
	{
		m[0]=0;
		m[1]=1;
	}
	else if (value <= 0xFFFFFFFFU && value==(double)(uint32)value)
	{
		m[0]=(uint32)value;
		m[1]=1;
	}
	else if (value<1.0)
	{
		m[0]=(uint32)(value*0xFFFFFFFF);
		m[1]=0xFFFFFFFF;
	}
	else
	{
		m[0]=0xFFFFFFFF;
		m[1]=(uint32)(0xFFFFFFFF/value);
	}
	if (tif->tif_flags&TIFF_SWAB)
	{
		TIFFSwabLong(&m[0]);
		TIFFSwabLong(&m[1]);
	}
	return(TIFFWriteDirectoryTagData(tif,ndir,dir,tag,TIFF_RATIONAL,1,8,&m[0]));
}