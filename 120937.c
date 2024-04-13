static INLINE REGION16_DATA* allocateRegion(long nbItems)
{
	long allocSize = sizeof(REGION16_DATA) + (nbItems * sizeof(RECTANGLE_16));
	REGION16_DATA* ret = (REGION16_DATA*) malloc(allocSize);

	if (!ret)
		return ret;

	ret->size = allocSize;
	ret->nbRects = nbItems;
	return ret;
}