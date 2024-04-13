toggled_colors(int c)
{
    if (c < 16) {
	static const int table[] =
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};
	c = table[c];
    }
    return c;
}