static ut32 thumb_getshift(const char *str) {
	// only immediate shifts are ever used by thumb-2. Bit positions are different from ARM.
	const char *shifts[] = {
		"LSL", "LSR", "ASR", "ROR", 0, "RRX"
	};
	char *type = strdup (str);
	char *arg;
	char *space;
	ut32 res = 0;
	ut32 shift = false;
	err = false;
	ut32 argn;
	ut32 i;
	
	r_str_case (type,true);
	
	if (!strcmp (type, shifts[5])) {
		// handle RRX alias case
		res |= 3 << 12;	
		free (type);
		return res;
	}
	
	space = strchr (type, ' ');
	if (!space) {
		free (type);
		err = true;
		return 0;
	}
	*space = 0;
	arg = strdup (++space);
	
	for (i = 0; shifts[i]; i++) {
		if (!strcmp (type, shifts[i])) {
			shift = true;
			break;
		}
	}
	if (!shift) {
		err = true;
		free (type);
		free (arg);
		return 0;
	}
	res |= i << 12;
		
	argn = getnum (arg);
	if (err || argn > 32) {
		err = true;
		free (type);
		free (arg);
		return 0;
	}
	res |= ( (argn & 0x1c) << 2);
	res |= ( (argn & 0x3) << 14);

	free (type);
	free (arg);
	return res;
}