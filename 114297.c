const u32 *gf_isom_get_brands(GF_ISOFile *movie)
{
	if (!movie || !movie->brand) return NULL;
	return movie->brand->altBrand;
}