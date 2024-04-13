TIFFReadDirectoryFindFieldInfo(TIFF* tif, uint16 tagid, uint32* fii)
{
	int32 ma,mb,mc;
	ma=-1;
	mc=(int32)tif->tif_nfields;
	while (1)
	{
		if (ma+1==mc)
		{
			*fii = FAILED_FII;
			return;
		}
		mb=(ma+mc)/2;
		if (tif->tif_fields[mb]->field_tag==(uint32)tagid)
			break;
		if (tif->tif_fields[mb]->field_tag<(uint32)tagid)
			ma=mb;
		else
			mc=mb;
	}
	while (1)
	{
		if (mb==0)
			break;
		if (tif->tif_fields[mb-1]->field_tag!=(uint32)tagid)
			break;
		mb--;
	}
	*fii=mb;
}