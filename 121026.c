void region16_print(const REGION16* region)
{
	const RECTANGLE_16* rects;
	UINT32 nbRects, i;
	int currentBandY = -1;
	rects = region16_rects(region, &nbRects);
	WLog_DBG(TAG,  "nrects=%"PRIu32"", nbRects);

	for (i = 0; i < nbRects; i++, rects++)
	{
		if (rects->top != currentBandY)
		{
			currentBandY = rects->top;
			WLog_DBG(TAG,  "band %d: ", currentBandY);
		}

		WLog_DBG(TAG,  "(%"PRIu16",%"PRIu16"-%"PRIu16",%"PRIu16")", rects->left, rects->top, rects->right,
		         rects->bottom);
	}
}