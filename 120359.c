
void paen_del(GF_Box *s)
{
	FDPartitionEntryBox *ptr = (FDPartitionEntryBox *)s;
	if (ptr == NULL) return;
	if (ptr->blocks_and_symbols) gf_isom_box_del((GF_Box*)ptr->blocks_and_symbols);
	if (ptr->FEC_symbol_locations) gf_isom_box_del((GF_Box*)ptr->FEC_symbol_locations);
	if (ptr->File_symbol_locations) gf_isom_box_del((GF_Box*)ptr->File_symbol_locations);
	gf_free(ptr);