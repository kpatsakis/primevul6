GF_Err tfra_Size(GF_Box *s)
{
	GF_TrackFragmentRandomAccessBox *ptr = (GF_TrackFragmentRandomAccessBox *)s;

	ptr->size += 12;

	ptr->size += ptr->nb_entries * ( ((ptr->version==1) ? 16 : 8 ) + ptr->traf_bits/8 + ptr->trun_bits/8 + ptr->sample_bits/8);
	return GF_OK;
}