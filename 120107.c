GF_Err drep_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_DREPBox *ptr = (GF_DREPBox *)s;
	ptr->nbBytes = gf_bs_read_u64(bs);
	return GF_OK;
}