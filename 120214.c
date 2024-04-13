
GF_Err trgt_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackGroupTypeBox *ptr = (GF_TrackGroupTypeBox *)s;
	ptr->track_group_id = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);
	return GF_OK;