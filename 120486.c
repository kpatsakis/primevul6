GF_Err maxr_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MAXRBox *ptr = (GF_MAXRBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->granularity);
	gf_bs_write_u32(bs, ptr->maxDataRate);
	return GF_OK;
}