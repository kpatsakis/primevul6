GF_Err iods_Size(GF_Box *s)
{
	GF_ObjectDescriptorBox *ptr = (GF_ObjectDescriptorBox *)s;

	ptr->size += gf_odf_desc_size(ptr->descriptor);
	return GF_OK;
}