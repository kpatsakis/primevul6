static st32 getregmemstart(const char *input) {
	if ((strlen (input) < 1) || (!(*input == '['))) {
		return -1;
	}
	input++;
	return getreg (input);
}