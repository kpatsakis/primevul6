
GF_Err vmhd_Size(GF_Box *s)
{
	GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;
	ptr->size += 8;
	return GF_OK;