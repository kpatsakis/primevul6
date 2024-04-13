GF_Err dref_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_DataReferenceBox *ptr = (GF_DataReferenceBox *)s;

	if (ptr == NULL) return GF_BAD_PARAM;
	gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	return gf_isom_box_array_read(s, bs, dref_AddDataEntry);
}