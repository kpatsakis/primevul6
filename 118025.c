void display(const char *str) {
	char cmd[32];
	int op = armass_assemble (str, 0x1000, 0);
	printf ("[%08x] %s\n", op, str);
	snprintf (cmd, sizeof(cmd), "rasm2 -d -a arm %08x", op);
	system (cmd);
}