GF_Box *nmhd_New()
{
	ISOM_DECL_BOX_ALLOC(GF_MPEGMediaHeaderBox, GF_ISOM_BOX_TYPE_NMHD);
	return (GF_Box *)tmp;
}