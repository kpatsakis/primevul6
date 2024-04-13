GF_Err minf_Size(GF_Box *s)
{
	GF_Err e;
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;

	if (ptr->InfoHeader) {
		e = gf_isom_box_size((GF_Box *) ptr->InfoHeader);
		if (e) return e;
		ptr->size += ptr->InfoHeader->size;
	}
	if (ptr->dataInformation) {
		e = gf_isom_box_size((GF_Box *) ptr->dataInformation);
		if (e) return e;
		ptr->size += ptr->dataInformation->size;
	}
	if (ptr->sampleTable) {
		e = gf_isom_box_size((GF_Box *) ptr->sampleTable);
		if (e) return e;
		ptr->size += ptr->sampleTable->size;
	}
	return GF_OK;
}