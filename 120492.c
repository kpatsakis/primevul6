void mdia_del(GF_Box *s)
{
	GF_MediaBox *ptr = (GF_MediaBox *)s;
	if (ptr == NULL) return;
	if (ptr->mediaHeader) gf_isom_box_del((GF_Box *)ptr->mediaHeader);
	if (ptr->information) gf_isom_box_del((GF_Box *)ptr->information);
	if (ptr->handler) gf_isom_box_del((GF_Box *)ptr->handler);
	gf_free(ptr);
}