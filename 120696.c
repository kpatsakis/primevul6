void smhd_del(GF_Box *s)
{
	GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;
	if (ptr == NULL ) return;
	gf_free(ptr);
}