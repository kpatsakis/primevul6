static int r_cmd_java_print_class_access_flags_value( const char * flags ){
	ut16 result = r_bin_java_calculate_class_access_value (flags);
	r_cons_printf ("Access Value for %s = 0x%04x\n", flags, result);
	return true;
}