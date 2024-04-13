GF_Err rely_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_RelyHintBox *ptr = (GF_RelyHintBox *)s;
	ptr->reserved = gf_bs_read_int(bs, 6);
	ptr->prefered = gf_bs_read_int(bs, 1);
	ptr->required = gf_bs_read_int(bs, 1);
	return GF_OK;
}