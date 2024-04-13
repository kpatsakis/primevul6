
GF_Err txtc_Size(GF_Box *s)
{
	GF_TextConfigBox *ptr = (GF_TextConfigBox *)s;
	if (ptr->config)
		ptr->size += strlen(ptr->config);
	ptr->size++;
	return GF_OK;