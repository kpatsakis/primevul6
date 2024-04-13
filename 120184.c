
void trep_del(GF_Box *s)
{
	GF_TrackExtensionPropertiesBox *ptr = (GF_TrackExtensionPropertiesBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);