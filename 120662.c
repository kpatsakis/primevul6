
GF_Err prft_Size(GF_Box *s)
{
	GF_ProducerReferenceTimeBox *ptr = (GF_ProducerReferenceTimeBox*)s;

	ptr->size += 4+8+ (ptr->version ? 8 : 4);
	return GF_OK;