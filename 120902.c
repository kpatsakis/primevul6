int region16_n_rects(const REGION16* region)
{
	assert(region);
	assert(region->data);
	return region->data->nbRects;
}