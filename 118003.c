static inline int arm_opcode_cond(ArmOpcode *ao, int delta) {
	const char *conds[] = {
		"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
		"hi", "ls", "ge", "lt", "gt", "le", "al", "nv", 0
	};
	int i, cond = 14; // 'always' is default
	char *c = ao->op+delta;
	for (i=0; conds[i]; i++) {
		if (!strcmpnull (c, conds[i])) {
			cond = i;
			break;
		}
	}
	ao->o |= cond << 4;
	return cond;
}