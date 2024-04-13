GF_Err srpp_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SRTPProcessBox *ptr = (GF_SRTPProcessBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->encryption_algorithm_rtp);
	gf_bs_write_u32(bs, ptr->encryption_algorithm_rtcp);
	gf_bs_write_u32(bs, ptr->integrity_algorithm_rtp);
	gf_bs_write_u32(bs, ptr->integrity_algorithm_rtcp);
	if (ptr->info) {
		e = gf_isom_box_write((GF_Box*)ptr->info, bs);
		if (e) return e;
	}
	if (ptr->scheme_type) {
		e = gf_isom_box_write((GF_Box*)ptr->scheme_type, bs);
		if (e) return e;
	}
	return GF_OK;
}