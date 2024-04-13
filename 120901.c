const RECTANGLE_16* region16_extents(const REGION16* region)
{
	if (!region)
		return NULL;

	return &region->extents;
}