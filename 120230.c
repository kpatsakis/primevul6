GF_Err snro_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_SeqOffHintEntryBox *ptr = (GF_SeqOffHintEntryBox *)s;
	ptr->SeqOffset = gf_bs_read_u32(bs);
	return GF_OK;
}