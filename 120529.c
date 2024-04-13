
GF_Err pasp_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_PixelAspectRatioBox *ptr = (GF_PixelAspectRatioBox*)s;
	ptr->hSpacing = gf_bs_read_u32(bs);
	ptr->vSpacing = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 8);
	return GF_OK;