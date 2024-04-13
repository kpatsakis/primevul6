GF_Err dimm_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_DIMMBox *ptr = (GF_DIMMBox *)s;
	ptr->nbBytes = gf_bs_read_u64(bs);
	return GF_OK;
}