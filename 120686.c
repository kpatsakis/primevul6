GF_Err kind_Size(GF_Box *s)
{
	GF_KindBox *ptr = (GF_KindBox *)s;

	ptr->size += strlen(ptr->schemeURI) + 1;
	if (ptr->value) {
		ptr->size += strlen(ptr->value) + 1;
	}
	return GF_OK;
}