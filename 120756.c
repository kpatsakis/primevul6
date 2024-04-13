
GF_Err tsro_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TimeOffHintEntryBox *ptr = (GF_TimeOffHintEntryBox *)s;
	ptr->TimeOffset = gf_bs_read_u32(bs);
	return GF_OK;