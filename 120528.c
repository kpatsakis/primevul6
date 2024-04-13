GF_Err totl_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TOTLBox *ptr = (GF_TOTLBox *)s;
	ptr->nbBytes = gf_bs_read_u32(bs);
	return GF_OK;
}