static int check_pe64_bytes(const ut8 *buf, ut64 length) {
	int idx, ret = false;
	if (!buf || length <= 0x3d) {
		return false;
	}
	idx = buf[0x3c] | (buf[0x3d]<<8);
	if (length >= idx + 0x20) {
		if (!memcmp (buf, "MZ", 2) && !memcmp (buf+idx, "PE", 2) && !memcmp (buf+idx+0x18, "\x0b\x02", 2)) {
			ret = true;
		}
	}
	return ret;
}