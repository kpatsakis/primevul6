GF_Err npck_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_NPCKBox *ptr = (GF_NPCKBox *)s;
	ptr->nbPackets = gf_bs_read_u32(bs);
	return GF_OK;
}