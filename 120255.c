GF_Err nump_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_NUMPBox *ptr = (GF_NUMPBox *)s;
	ptr->nbPackets = gf_bs_read_u64(bs);
	return GF_OK;
}