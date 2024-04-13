static void r_bin_dwarf_free_attr_value(RBinDwarfAttrValue *val) {
	if (!val) {
		return;
	}
	switch (val->form) {
	case DW_FORM_strp:
	case DW_FORM_string:
		R_FREE (val->encoding.str_struct.string);
		break;
	case DW_FORM_block:
	case DW_FORM_block1:
	case DW_FORM_block2:
	case DW_FORM_block4:
		R_FREE (val->encoding.block.data);
		break;
	default:
		break;
	};
}