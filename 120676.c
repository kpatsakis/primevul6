GF_Err dref_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 count;
	GF_DataReferenceBox *ptr = (GF_DataReferenceBox *)s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	count = ptr->other_boxes ? gf_list_count(ptr->other_boxes) : 0;
	gf_bs_write_u32(bs, count);
	return GF_OK;
}