
void tref_del(GF_Box *s)
{
	GF_TrackReferenceBox *ptr = (GF_TrackReferenceBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);