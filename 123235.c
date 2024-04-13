static char r_cmd_java_is_valid_java_mcf (char b) {
	char c = 0;
	switch (b) {
		case 'c':
		case 'f':
		case 'm': c = b;
	}
	return c;
}