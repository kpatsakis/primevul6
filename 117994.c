static st8 iflag(char *input) {
	st8 res = 0;
	ut8 i;
	r_str_case (input, false);
	
	for (i = 0; i < strlen(input); i++) {
		switch (input[i]) {
		case 'a':
			res |= 0x4;
			break;
		case 'i':
			res |= 0x2;
			break;
		case 'f':
			res |= 0x1;
			break;
		default:
			return -1;
		}
	}
	return res;
}