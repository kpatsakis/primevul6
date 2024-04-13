GF_Err rssr_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_ReceivedSsrcBox *ptr = (GF_ReceivedSsrcBox *)s;
	ptr->ssrc = gf_bs_read_u32(bs);
	return GF_OK;
}