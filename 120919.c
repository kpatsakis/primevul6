static RECTANGLE_16* next_band(RECTANGLE_16* band1, RECTANGLE_16* endPtr, int* nbItems)
{
	UINT16 refY = band1->top;
	*nbItems = 0;

	while ((band1 < endPtr) && (band1->top == refY))
	{
		band1++;
		*nbItems += 1;
	}

	return band1;
}