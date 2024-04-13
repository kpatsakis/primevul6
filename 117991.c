static void arm_opcode_parse(ArmOpcode *ao, const char *str) {
	int i;
	memset (ao, 0, sizeof (ArmOpcode));
	if (strlen (str) + 1 >= sizeof (ao->op)) {
		return;
	}
	strncpy (ao->op, str, sizeof (ao->op)-1);
	strcpy (ao->opstr, ao->op);
	ao->a[0] = strchr (ao->op, ' ');
	for (i=0; i<15; i++) {
		if (ao->a[i]) {
			*ao->a[i] = 0;
			ao->a[i+1] = strchr (++ao->a[i], ',');
		} else {
			break;
		}
	}
	if (ao->a[i]) {
		*ao->a[i] = 0;
		ao->a[i]++;
	}
	for (i=0; i<16; i++) {
		while (ao->a[i] && *ao->a[i] == ' ') {
			ao->a[i]++;
		}
	}
}