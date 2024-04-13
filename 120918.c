static BOOL band_match(const RECTANGLE_16* band1, const RECTANGLE_16* band2, RECTANGLE_16* endPtr)
{
	int refBand2 = band2->top;
	const RECTANGLE_16* band2Start = band2;

	while ((band1 < band2Start) && (band2 < endPtr) && (band2->top == refBand2))
	{
		if ((band1->left != band2->left) || (band1->right != band2->right))
			return FALSE;

		band1++;
		band2++;
	}

	if (band1 != band2Start)
		return FALSE;

	return (band2 == endPtr) || (band2->top != refBand2);
}