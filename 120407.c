
GF_Err trik_Size(GF_Box *s)
{
	GF_TrickPlayBox *ptr = (GF_TrickPlayBox *) s;
	ptr->size += 8 * ptr->entry_count;
	return GF_OK;