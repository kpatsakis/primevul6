static int check_pe32_bytes(const ut8 *buf, ut64 length) {
	unsigned int idx;
	if (!buf || length <= 0x3d) {
		return false;
	}
	idx = (buf[0x3c] | (buf[0x3d]<<8));
	if (length > idx + 0x18 + 2) {
		if (!memcmp (buf, "MZ", 2) && !memcmp (buf+idx, "PE", 2) && !memcmp (buf+idx+0x18, "\x0b\x01", 2)) {
			return true;
		}
	}
	return false;
}