R_API void r_bin_java_print_local_variable_type_attr_summary(RBinJavaLocalVariableTypeAttribute *lvattr) {
	if (!lvattr) {
		eprintf ("Attempting to print an invalid RBinJavaLocalVariableTypeAttribute *.\n");
		return;
	}
	eprintf ("   Local Variable Type Attribute offset: 0x%08"PFMT64x "\n", lvattr->file_offset);
	eprintf ("   Local Variable Type Attribute start_pc: %d\n", lvattr->start_pc);
	eprintf ("   Local Variable Type Attribute Length: %d\n", lvattr->length);
	eprintf ("   Local Variable Type Attribute name_idx: %d\n", lvattr->name_idx);
	eprintf ("   Local Variable Type Attribute name: %s\n", lvattr->name);
	eprintf ("   Local Variable Type Attribute signature_idx: %d\n", lvattr->signature_idx);
	eprintf ("   Local Variable Type Attribute signature: %s\n", lvattr->signature);
	eprintf ("   Local Variable Type Attribute index: %d\n", lvattr->index);
}