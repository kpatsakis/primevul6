SetCharacterClassRange(int low,	/* in range of [0..255] */
		       int high,
		       int value)	/* arbitrary */
{

    if (low < 0 || high > 255 || high < low)
	return (-1);

    for (; low <= high; low++)
	charClass[low] = value;

    return (0);
}