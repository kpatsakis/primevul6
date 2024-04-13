
GF_Err trep_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackExtensionPropertiesBox *ptr = (GF_TrackExtensionPropertiesBox *)s;

	ptr->trackID = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	return gf_isom_box_array_read(s, bs, gf_isom_box_add_default);