
GF_Err stri_Read(GF_Box *s, GF_BitStream *bs)
{
	size_t i;
	GF_SubTrackInformationBox *ptr = (GF_SubTrackInformationBox *)s;
	ptr->switch_group = gf_bs_read_u16(bs);
	ptr->alternate_group = gf_bs_read_u16(bs);
	ptr->sub_track_id = gf_bs_read_u32(bs);
	ptr->size -= 8;
	ptr->attribute_count = ptr->size / 4;
	GF_SAFE_ALLOC_N(ptr->attribute_list, (size_t)ptr->attribute_count, u32);
	if (!ptr->attribute_list) return GF_OUT_OF_MEM;
	for (i = 0; i < ptr->attribute_count; i++) {
		ptr->attribute_list[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;