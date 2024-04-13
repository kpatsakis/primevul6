
void trex_del(GF_Box *s)
{
	GF_TrackExtendsBox *ptr = (GF_TrackExtendsBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);