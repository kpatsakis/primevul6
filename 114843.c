void real_block_target(unsigned char *target, const unsigned char *data)
{
	uint8_t targetshift;

	if (unlikely(data[72] < 3 || data[72] > 0x20))
	{
		// Invalid (out of bounds) target
		memset(target, 0xff, 32);
		return;
	}

	targetshift = data[72] - 3;
	memset(target, 0, targetshift);
	target[targetshift++] = data[75];
	target[targetshift++] = data[74];
	target[targetshift++] = data[73];
	memset(&target[targetshift], 0, 0x20 - targetshift);
}