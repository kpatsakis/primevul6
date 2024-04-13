
GF_Err bloc_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_BaseLocationBox *ptr = (GF_BaseLocationBox *) s;

	ISOM_DECREASE_SIZE(s, 256)
	gf_bs_read_data(bs, (char *) ptr->baseLocation, 256);
	ISOM_DECREASE_SIZE(s, 256)
	gf_bs_read_data(bs, (char *) ptr->basePurlLocation, 256);
	ISOM_DECREASE_SIZE(s, 512)
	gf_bs_skip_bytes(bs, 512);
	return GF_OK;