
GF_Err paen_Size(GF_Box *s)
{
	GF_Err e;
	FDPartitionEntryBox *ptr = (FDPartitionEntryBox *)s;

	if (ptr->blocks_and_symbols) {
		e = gf_isom_box_size((GF_Box *)ptr->blocks_and_symbols);
		if (e) return e;
		ptr->size += ptr->blocks_and_symbols->size;
	}
	if (ptr->FEC_symbol_locations) {
		e = gf_isom_box_size((GF_Box *) ptr->FEC_symbol_locations);
		if (e) return e;
		ptr->size += ptr->FEC_symbol_locations->size;
	}
	if (ptr->File_symbol_locations) {
		e = gf_isom_box_size((GF_Box *) ptr->File_symbol_locations);
		if (e) return e;
		ptr->size += ptr->File_symbol_locations->size;
	}

	return GF_OK;