GF_Err minf_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	//Header first
	if (ptr->InfoHeader) {
		e = gf_isom_box_write((GF_Box *) ptr->InfoHeader, bs);
		if (e) return e;
	}
	//then dataInfo
	if (ptr->dataInformation) {
		e = gf_isom_box_write((GF_Box *) ptr->dataInformation, bs);
		if (e) return e;
	}
	//then sampleTable
	if (ptr->sampleTable) {
		e = gf_isom_box_write((GF_Box *) ptr->sampleTable, bs);
		if (e) return e;
	}
	return GF_OK;
}