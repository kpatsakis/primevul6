GF_Err uuid_Size(GF_Box *s)
{
	GF_UnknownUUIDBox*ptr = (GF_UnknownUUIDBox*)s;
	ptr->size += ptr->dataSize;
	return GF_OK;
}