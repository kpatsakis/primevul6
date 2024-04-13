
void strk_del(GF_Box *s)
{
	GF_SubTrackBox *ptr = (GF_SubTrackBox *)s;
	if (ptr == NULL) return;
	if (ptr->info) gf_isom_box_del((GF_Box *)ptr->info);
	gf_free(ptr);