uint64_t readValue(struct READER *reader, int size) {
	int i, c;
	uint64_t value;
	c = fgetc(reader->fhd);
	if (c < 0)
		return 0xffffffffffffffffLL;
	value = (uint8_t) c;
	for (i = 1; i < size; i++) {
		c = fgetc(reader->fhd);
		if (c < 0)
			return 0xffffffffffffffffLL;
		value |= ((uint64_t) c) << (i * 8);
	}
	return value;
}