
GF_Err metx_Size(GF_Box *s)
{
	GF_Err e;
	GF_MetaDataSampleEntryBox *ptr = (GF_MetaDataSampleEntryBox *)s;
	ptr->size += 8;

	if (ptr->type!=GF_ISOM_BOX_TYPE_STPP) {
		if (ptr->content_encoding)
			ptr->size += strlen(ptr->content_encoding);
		ptr->size++;
	}

	if ((ptr->type==GF_ISOM_BOX_TYPE_METX) || (ptr->type==GF_ISOM_BOX_TYPE_STPP)) {

		if (ptr->xml_namespace)
			ptr->size += strlen(ptr->xml_namespace);
		ptr->size++;

		if (ptr->xml_schema_loc)
			ptr->size += strlen(ptr->xml_schema_loc);
		ptr->size++;

		if (ptr->type==GF_ISOM_BOX_TYPE_STPP) {
			if (ptr->mime_type)
				ptr->size += strlen(ptr->mime_type);
			ptr->size++;
		}

	}
	//mett, sbtt, stxt
	else {
		if (ptr->mime_type)
			ptr->size += strlen(ptr->mime_type);
		ptr->size++;

		if (ptr->config) {
			e = gf_isom_box_size((GF_Box *)ptr->config);
			if (e) return e;
			ptr->size += ptr->config->size;
		}
	}
	return gf_isom_box_array_size(s, ptr->protections);