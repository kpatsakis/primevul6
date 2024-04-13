void mvex_del(GF_Box *s)
{
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *)s;
	if (ptr == NULL) return;
	if (ptr->mehd) gf_isom_box_del((GF_Box*)ptr->mehd);
	gf_isom_box_array_del(ptr->TrackExList);
	gf_isom_box_array_del(ptr->TrackExPropList);
	ptr->mehd = NULL;
	ptr->TrackExList = NULL;
	ptr->TrackExPropList = NULL;
	gf_free(ptr);
}