static void dump_r_bin_dwarf_debug_abbrev(FILE *f, RBinDwarfDebugAbbrev *da) {
	size_t i, j;
	ut64 attr_name, attr_form;

	if (!f || !da) {
		return;
	}
	for (i = 0; i < da->length; i++) {
		int declstag = da->decls[i].tag;
		fprintf (f, "Abbreviation Code %"PFMT64d" ", da->decls[i].code);
		if (declstag>=0 && declstag < DW_TAG_LAST) {
			fprintf (f, "Tag %s ", dwarf_tag_name_encodings[declstag]);
		}
		fprintf (f, "[%s]\n", da->decls[i].has_children ?
				"has children" : "no children");
		fprintf (f, "Offset 0x%"PFMT64x"\n", da->decls[i].offset);

		if (da->decls[i].specs) {
			for (j = 0; j < da->decls[i].length; j++) {
				attr_name = da->decls[i].specs[j].attr_name;
				attr_form = da->decls[i].specs[j].attr_form;
				if (attr_name && attr_form &&
					attr_name <= DW_AT_vtable_elem_location &&
					attr_form <= DW_FORM_indirect) {
					fprintf (f, "    %s %s\n",
							dwarf_attr_encodings[attr_name],
							dwarf_attr_form_encodings[attr_form]);
				}
			}
		}
	}
}