GF_Err mdia_Size(GF_Box *s)
{
	GF_Err e;
	GF_MediaBox *ptr = (GF_MediaBox *)s;

	if (ptr->mediaHeader) {
		e = gf_isom_box_size((GF_Box *) ptr->mediaHeader);
		if (e) return e;
		ptr->size += ptr->mediaHeader->size;
	}
	if (ptr->handler) {
		e = gf_isom_box_size((GF_Box *) ptr->handler);
		if (e) return e;
		ptr->size += ptr->handler->size;
	}
	if (ptr->information) {
		e = gf_isom_box_size((GF_Box *) ptr->information);
		if (e) return e;
		ptr->size += ptr->information->size;
	}
	return GF_OK;
}