MotionButton(unsigned state)
{
    unsigned bmask = state >> Button1Index;
    int result = 1;

    if (bmask != 0) {
	while (!(bmask & 1)) {
	    ++result;
	    bmask >>= 1;
	}
    }
    return result;
}