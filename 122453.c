static void print_octets(uint8_t *raw, int len, void (*print)(const char *fmt, ...))
{
	print(" 0x");
	while (len--)
		print("%02x", *raw++);
}