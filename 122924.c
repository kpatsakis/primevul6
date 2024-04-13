GF_Err vvnc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_VVCNaluConfigurationBox *ptr = (GF_VVCNaluConfigurationBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, 0, 6);
	gf_bs_write_int(bs, ptr->nal_unit_size-1, 2);
	return GF_OK;
}