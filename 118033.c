static bool parseOpcode(const char *str, ArmOp *op) {
	char *in = strdup (str);
	char *space = strchr (in, ' ');
	if (!space) {
		op->operands[0].type = ARM_NOTYPE;
		op->mnemonic = in;
 		return true;
	}
	space[0] = '\0';
	op->mnemonic = in;
	space ++;
	return parseOperands (space, op);
}