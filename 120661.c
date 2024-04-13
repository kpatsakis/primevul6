
GF_Err stsg_Size(GF_Box *s)
{
	GF_SubTrackSampleGroupBox *ptr = (GF_SubTrackSampleGroupBox *)s;
	ptr->size += 6 + 4 * ptr->nb_groups;
	return GF_OK;