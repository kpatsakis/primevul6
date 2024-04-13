BOOL region16_is_empty(const REGION16* region)
{
	assert(region);
	assert(region->data);
	return (region->data->nbRects == 0);
}