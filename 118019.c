void collect_list(char *input[]) {
	if (input[0] == NULL) {
		return;
	}
	char *temp  = malloc (500);
	if (!temp) {
		return;
	}
	temp[0] = 0;
	int i;
	int conc = 0;
	int start, end = 0;
	int arrsz;
	for (arrsz = 1; input[arrsz] != NULL; arrsz++) {
		;
	}

	for (i = 0; input[i]; i++) {
		if (conc) {
			strcat (temp, ", ");
			strcat (temp, input[i]);
		}
		if (input[i][0] == '{') {
			conc = 1;
			strcat (temp, input[i]);
			start = i;
		}
		if ((conc) & (input[i][strlen (input[i]) - 1] == '}')) {
			conc = 0;
			end = i;
		}
	}
	if (end == 0) {
		free (temp);
		return;
	}
	input[start] = temp;
	for (i = start + 1; i < arrsz; i++) {
		input[i] = input[(end-start) + i];
	}
	input[i] = NULL;
}