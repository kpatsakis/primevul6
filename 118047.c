static ut32 getshift(const char *str) {
	char type[128];
	char arg[128];
	char *space;
	ut32 i=0, shift=0;
	const char *shifts[] = {
		"LSL", "LSR", "ASR", "ROR",
		0, "RRX" // alias for ROR #0
	};

	strncpy (type, str, sizeof (type) - 1);
	// XXX strcaecmp is probably unportable
	if (!r_str_casecmp (type, shifts[5])) {
		// handle RRX alias case
		shift = 6;
	} else { // all other shift types
		space = strchr (type, ' ');
		if (!space) {
			return 0;
		}
		*space = 0;
		strncpy (arg, ++space, sizeof(arg) - 1);

		for (i = 0; shifts[i]; i++) {
			if (!r_str_casecmp (type, shifts[i])) {
				shift = 1;
				break;
			}
		}
		if (!shift) {
			return 0;
		}
		shift = i * 2;
		if ((i = getreg (arg)) != -1) {
			i <<= 8; // set reg
//			i|=1; // use reg
			i |= (1 << 4); // bitshift
			i |= shift << 4; // set shift mode
			if (shift == 6) {
				i |= (1 << 20);
			}
		} else {
			char *bracket = strchr (arg, ']');
			if (bracket) {
				*bracket = '\0';
			}
			// ensure only the bottom 5 bits are used
			i &= 0x1f;
			if (!i) {
				i = 32;
			}
			i = (i * 8);
			i |= shift; // lsl, ror, ...
			i = i << 4;
		}
	}

	return i;
}