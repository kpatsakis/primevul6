
GF_Err trex_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackExtendsBox *ptr = (GF_TrackExtendsBox *)s;

	ptr->trackID = gf_bs_read_u32(bs);
	ptr->def_sample_desc_index = gf_bs_read_u32(bs);
	ptr->def_sample_duration = gf_bs_read_u32(bs);
	ptr->def_sample_size = gf_bs_read_u32(bs);
	ptr->def_sample_flags = gf_bs_read_u32(bs);
	return GF_OK;