GF_Box *stts_New()
{
	ISOM_DECL_BOX_ALLOC(GF_TimeToSampleBox, GF_ISOM_BOX_TYPE_STTS);
	return (GF_Box *)tmp;
}