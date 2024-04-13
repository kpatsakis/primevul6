GF_Err co64_Size(GF_Box *s)
{
	GF_ChunkLargeOffsetBox *ptr = (GF_ChunkLargeOffsetBox *) s;

	ptr->size += 4 + (8 * ptr->nb_entries);
	return GF_OK;
}