GF_Box *stsz_New()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleSizeBox, 0);

	//type is unknown here, can be regular or compact table
	return (GF_Box *)tmp;
}