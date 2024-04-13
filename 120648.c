
GF_Err pcrb_Read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_PcrInfoBox *ptr = (GF_PcrInfoBox*) s;

	ptr->subsegment_count = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	ptr->pcr_values = gf_malloc(sizeof(u64)*ptr->subsegment_count);
	if (!ptr->pcr_values)
	    return GF_OUT_OF_MEM;
	for (i=0; i<ptr->subsegment_count; i++) {
		u64 data1 = gf_bs_read_u32(bs);
		u64 data2 = gf_bs_read_u16(bs);
		ISOM_DECREASE_SIZE(ptr, 6);
		ptr->pcr_values[i] = (data1 << 10) | (data2 >> 6);

	}
	return GF_OK;