R_API void r_bin_java_print_interfacemethodref_cp_summary(RBinJavaCPTypeObj *obj) {
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  InterfaceMethodRef.\n");
		return;
	}
	eprintf ("InterfaceMethodRef ConstantPool Type (%d) ", obj->metas->ord);
	eprintf ("	Offset: 0x%08"PFMT64x"", obj->file_offset);
	eprintf ("	Class Index = %d\n", obj->info.cp_interface.class_idx);
	eprintf ("	Name and type Index = %d\n", obj->info.cp_interface.name_and_type_idx);
}