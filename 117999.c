static ut32 getimmed8(const char *str) {
	ut32 num = getnum (str);
	if (err) {
		return 0;
	}
	ut32 rotate;
	if (num <= 0xff) {
		return num;
	} else {
		for (rotate = 1; rotate < 16; rotate++) {
			// rol 2
			num = ((num << 2) | (num >> 30));
			if (num == (num & 0xff)) {
				return (num | (rotate << 8));
			}
		}
		err = 1;
		return 0;
	}
}