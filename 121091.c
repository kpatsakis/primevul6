void region16_init(REGION16* region)
{
	assert(region);
	ZeroMemory(region, sizeof(REGION16));
	region->data = &empty_region;
}