static void r_bin_dwarf_dump_attr_value(const RBinDwarfAttrValue *val, FILE *f) {
	size_t i;
	if (!val || !f) {
		return;
	}
	switch (val->form) {
	case DW_FORM_addr:
		fprintf (f, "0x%"PFMT64x"", val->encoding.address);
		break;
	case DW_FORM_block:
	case DW_FORM_block1:
	case DW_FORM_block2:
	case DW_FORM_block4:
		fprintf (f, "%"PFMT64u" byte block:", val->encoding.block.length);
		for (i = 0; i < val->encoding.block.length; i++) {
			fprintf (f, "%02x", val->encoding.block.data[i]);
		}
		break;
	case DW_FORM_data1:
	case DW_FORM_data2:
	case DW_FORM_data4:
	case DW_FORM_data8:
		fprintf (f, "%"PFMT64u"", val->encoding.data);
		if (val->name == DW_AT_language) {
			fprintf (f, "   (%s)", dwarf_langs[val->encoding.data]);
		}
		break;
	case DW_FORM_strp:
		fprintf (f, "(indirect string, offset: 0x%"PFMT64x"): ",
				val->encoding.str_struct.offset);
	case DW_FORM_string:
		if (val->encoding.str_struct.string) {
			fprintf (f, "%s", val->encoding.str_struct.string);
		} else {
			fprintf (f, "No string found");
		}
		break;
	case DW_FORM_flag:
		fprintf (f, "%u", val->encoding.flag);
		break;
	case DW_FORM_sdata:
		fprintf (f, "%"PFMT64d"", val->encoding.sdata);
		break;
	case DW_FORM_udata:
		fprintf (f, "%"PFMT64u"", val->encoding.data);
		break;
	case DW_FORM_ref_addr:
		fprintf (f, "<0x%"PFMT64x">", val->encoding.reference);
		break;
	case DW_FORM_ref1:
	case DW_FORM_ref2:
	case DW_FORM_ref4:
	case DW_FORM_ref8:
		fprintf (f, "<0x%"PFMT64x">", val->encoding.reference);
		break;
	default:
		fprintf (f, "Unknown attr value form %"PFMT64d"\n", val->form);
	};
}