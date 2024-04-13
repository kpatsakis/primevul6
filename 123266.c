static int r_cmd_java_handle_print_exceptions (RCore *core, const char *input) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *bin = (RBinJavaObj *) r_cmd_java_get_bin_obj (anal);
	RListIter *exc_iter = NULL, *methods_iter=NULL;
	RBinJavaField *method;
	ut64 func_addr = -1;
	RBinJavaExceptionEntry *exc_entry;

	const char *p = input? r_cmd_java_consumetok (input, ' ', -1): NULL;
	func_addr = p && *p && r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;

	if (!bin) {
		return false;
	}

	r_list_foreach (bin->methods_list, methods_iter, method) {
		ut64 start = r_bin_java_get_method_start(bin, method),
			end = r_bin_java_get_method_end(bin, method);
		ut8 do_this_one = start <= func_addr && func_addr <= end;	RList * exc_table = NULL;
		do_this_one = func_addr == -1 ? 1 : do_this_one;
		if (!do_this_one) {
			continue;
		}
		exc_table = r_bin_java_get_method_exception_table_with_addr (bin, start);

		if (r_list_length (exc_table) == 0){
			r_cons_printf (" Exception table for %s @ 0x%"PFMT64x":\n", method->name, start);
			r_cons_printf (" [ NONE ]\n");
		} else {
			r_cons_printf (" Exception table for %s (%d entries) @ 0x%"PFMT64x":\n", method->name,
				r_list_length (exc_table) , start);
		}
		r_list_foreach (exc_table, exc_iter, exc_entry) {
			char *class_info = r_bin_java_resolve_without_space (bin, exc_entry->catch_type);
			r_cons_printf ("  Catch Type: %d, %s @ 0x%"PFMT64x"\n", exc_entry->catch_type,
				class_info, exc_entry->file_offset+6);
			r_cons_printf ("  Start PC: (0x%"PFMT64x") 0x%"PFMT64x" @ 0x%"PFMT64x"\n",
				exc_entry->start_pc, exc_entry->start_pc+start, exc_entry->file_offset);
			r_cons_printf ("  End PC: (0x%"PFMT64x") 0x%"PFMT64x" 0x%"PFMT64x"\n",
				exc_entry->end_pc, exc_entry->end_pc+start, exc_entry->file_offset + 2);
			r_cons_printf ("  Handler PC: (0x%"PFMT64x") 0x%"PFMT64x" 0x%"PFMT64x"\n",
				exc_entry->handler_pc, exc_entry->handler_pc+start, exc_entry->file_offset+4);
			free (class_info);
		}
	}
	return true;
}