static char *parse_hints(char *input) {
	if (!strcmpnull (input, "unst")) {
		return "6";
	}
	if (!strcmpnull (input, "un")) {
		return "7";
	}
	if (!strcmpnull (input, "st")) {
		return "14";
	}
	if (!strcmpnull (input, "sy")) {
		return "15";
	}
	return "-1";
}