void edts_del(GF_Box *s)
{
	GF_EditBox *ptr = (GF_EditBox *) s;
	gf_isom_box_del((GF_Box *)ptr->editList);
	gf_free(ptr);
}