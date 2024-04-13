
GF_Err ssix_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i,j;
	GF_SubsegmentIndexBox *ptr = (GF_SubsegmentIndexBox*)s;

	if (ptr->size < 4) return GF_BAD_PARAM;
	ptr->subsegment_count = gf_bs_read_u32(bs);
	ptr->size -= 4;

	if (ptr->subsegment_count > UINT_MAX / sizeof(GF_SubsegmentInfo))
		return GF_ISOM_INVALID_FILE;

	GF_SAFE_ALLOC_N(ptr->subsegments, ptr->subsegment_count, GF_SubsegmentInfo);
	if (!ptr->subsegments)
	    return GF_OUT_OF_MEM;
	for (i = 0; i < ptr->subsegment_count; i++) {
		GF_SubsegmentInfo *subseg = &ptr->subsegments[i];
		if (ptr->size < 4) return GF_BAD_PARAM;
		subseg->range_count = gf_bs_read_u32(bs);
		ptr->size -= 4;
		if (ptr->size < subseg->range_count*4) return GF_BAD_PARAM;
		subseg->ranges = (GF_SubsegmentRangeInfo*) gf_malloc(sizeof(GF_SubsegmentRangeInfo) * subseg->range_count);
		for (j = 0; j < subseg->range_count; j++) {
			subseg->ranges[j].level = gf_bs_read_u8(bs);
			subseg->ranges[j].range_size = gf_bs_read_u24(bs);
			ptr->size -= 4;
		}
	}
	return GF_OK;