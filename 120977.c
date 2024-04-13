static INLINE RECTANGLE_16* region16_rects_noconst(REGION16* region)
{
	REGION16_DATA* data;
	data = region->data;

	if (!data)
		return NULL;

	return (RECTANGLE_16*)(&data[1]);
}