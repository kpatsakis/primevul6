void srpp_del(GF_Box *s)
{
	GF_SRTPProcessBox *ptr = (GF_SRTPProcessBox *)s;
	if (ptr->info) gf_isom_box_del((GF_Box*)ptr->info);
	if (ptr->scheme_type) gf_isom_box_del((GF_Box*)ptr->scheme_type);
	gf_free(s);
}