
GF_Err ainf_Size(GF_Box *s)
{
	GF_AssetInformationBox *ptr = (GF_AssetInformationBox *) s;
	s->size += 4 +  strlen(ptr->APID) + 1;
	return GF_OK;