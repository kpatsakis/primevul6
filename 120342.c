GF_Err dmed_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_DMEDBox *ptr = (GF_DMEDBox *)s;
	ptr->nbBytes = gf_bs_read_u64(bs);
	return GF_OK;
}