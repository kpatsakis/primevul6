void mdhd_del(GF_Box *s)
{
	GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}