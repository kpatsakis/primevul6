
void fdsa_del(GF_Box *s)
{
	GF_HintSample *ptr = (GF_HintSample *)s;
	gf_isom_box_array_del(ptr->packetTable);
	if (ptr->extra_data) gf_isom_box_del((GF_Box*)ptr->extra_data);
	gf_free(ptr);