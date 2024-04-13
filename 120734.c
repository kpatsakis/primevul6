GF_Err rtpo_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_RTPOBox *ptr = (GF_RTPOBox *)s;
	ptr->timeOffset = gf_bs_read_u32(bs);
	return GF_OK;
}