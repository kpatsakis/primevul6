GF_Err name_Size(GF_Box *s)
{
	GF_NameBox *ptr = (GF_NameBox *)s;
	if (ptr->string) ptr->size += strlen(ptr->string) + 1;
	return GF_OK;
}