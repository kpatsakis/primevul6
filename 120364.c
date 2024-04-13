GF_Err tims_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TSHintEntryBox *ptr = (GF_TSHintEntryBox *)s;
	ptr->timeScale = gf_bs_read_u32(bs);
	return GF_OK;
}