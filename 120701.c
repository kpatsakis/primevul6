GF_Err payt_Size(GF_Box *s)
{
	GF_PAYTBox *ptr = (GF_PAYTBox *)s;
	s->size += 4 + 1;
	if (ptr->payloadString) ptr->size += strlen(ptr->payloadString);
	return GF_OK;
}