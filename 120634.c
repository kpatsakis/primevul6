GF_Err dref_Size(GF_Box *s)
{
	GF_DataReferenceBox *ptr = (GF_DataReferenceBox *)s;
	if (!s) return GF_BAD_PARAM;

	ptr->size += 4;
	return GF_OK;
}