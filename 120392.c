GF_Err cprt_Size(GF_Box *s)
{
	GF_CopyrightBox *ptr = (GF_CopyrightBox *)s;

	ptr->size += 2;
	if (ptr->notice)
		ptr->size += strlen(ptr->notice) + 1;
	return GF_OK;
}