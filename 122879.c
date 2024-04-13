GF_Err vvnc_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_VVCNaluConfigurationBox *ptr = (GF_VVCNaluConfigurationBox *)s;

	ISOM_DECREASE_SIZE(ptr, 1)
	gf_bs_read_int(bs, 6);
	ptr->nal_unit_size = 1 + gf_bs_read_int(bs, 2);
	return GF_OK;
}