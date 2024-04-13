GF_Err tssy_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_TimeStampSynchronyBox *ptr = (GF_TimeStampSynchronyBox *)s;
	gf_bs_read_int(bs, 6);
	ptr->timestamp_sync = gf_bs_read_int(bs, 2);
	return GF_OK;
}