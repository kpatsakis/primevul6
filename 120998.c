static RECTANGLE_16* region16_extents_noconst(REGION16* region)
{
	if (!region)
		return NULL;

	return &region->extents;
}