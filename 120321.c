
GF_Err pcrb_Size(GF_Box *s)
{
	GF_PcrInfoBox *ptr = (GF_PcrInfoBox*) s;

	ptr->size += 4;
	ptr->size += ptr->subsegment_count * 6;

	return GF_OK;