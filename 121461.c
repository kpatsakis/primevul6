FirstBitN(int bits)
{
    int result = -1;
    if (bits > 0) {
	result = 0;
	while (!(bits & 1)) {
	    bits /= 2;
	    ++result;
	}
    }
    return result;
}