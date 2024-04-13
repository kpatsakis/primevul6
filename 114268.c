GF_Err gf_isom_get_alternate_brand(GF_ISOFile *movie, u32 BrandIndex, u32 *brand)
{
	if (!movie || !movie->brand || !brand) return GF_BAD_PARAM;
	if (BrandIndex > movie->brand->altCount || !BrandIndex) return GF_BAD_PARAM;
	*brand = movie->brand->altBrand[BrandIndex-1];
	return GF_OK;
}