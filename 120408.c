GF_Err unkn_Size(GF_Box *s)
{
	GF_UnknownBox *ptr = (GF_UnknownBox *)s;

	if (ptr->dataSize && ptr->data) {
		ptr->size += ptr->dataSize;
	}
	return GF_OK;
}