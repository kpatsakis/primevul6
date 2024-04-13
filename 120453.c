void minf_del(GF_Box *s)
{
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
	if (ptr == NULL) return;

	//if we have a Handler not self-contained, delete it (the self-contained belongs to the movie)
	if (ptr->dataHandler) {
		gf_isom_datamap_close(ptr);
	}
	if (ptr->InfoHeader) gf_isom_box_del((GF_Box *)ptr->InfoHeader);
	if (ptr->dataInformation) gf_isom_box_del((GF_Box *)ptr->dataInformation);
	if (ptr->sampleTable) gf_isom_box_del((GF_Box *)ptr->sampleTable);
	gf_free(ptr);
}