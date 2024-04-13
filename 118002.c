static st32 thumb_getoffset(char *label, ut64 cur) {
	st32 res = r_num_math (NULL, label);
	res -= 4;
	res -= cur; // possible integer underflow
	//printf("thumb_getoffset: %s, %lld, %lld\n", label, res, cur);
	return res;
}