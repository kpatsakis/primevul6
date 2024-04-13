reflection_cc_to_file (int call_conv) {
	switch (call_conv & 0x3) {
	case 0:
	case 1: return MONO_CALL_DEFAULT;
	case 2: return MONO_CALL_VARARG;
	default:
		g_assert_not_reached ();
	}
	return 0;
}