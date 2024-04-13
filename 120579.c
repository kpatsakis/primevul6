GF_Err trpy_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TRPYBox *ptr = (GF_TRPYBox *)s;
	ptr->nbBytes = gf_bs_read_u64(bs);
	return GF_OK;
}