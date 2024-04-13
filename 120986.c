const RECTANGLE_16* region16_rects(const REGION16* region, UINT32* nbRects)
{
	REGION16_DATA* data;

	if (nbRects)
		*nbRects = 0;

	if (!region)
		return NULL;

	data = region->data;

	if (!data)
		return NULL;

	if (nbRects)
		*nbRects = data->nbRects;

	return (RECTANGLE_16*)(data + 1);
}