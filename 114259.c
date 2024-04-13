GF_Err gf_isom_get_brand_info(GF_ISOFile *movie, u32 *brand, u32 *minorVersion, u32 *AlternateBrandsCount)
{
	if (!movie) return GF_BAD_PARAM;
	if (!movie->brand) {
		if (brand) *brand = GF_ISOM_BRAND_ISOM;
		if (minorVersion) *minorVersion = 1;
		if (AlternateBrandsCount) *AlternateBrandsCount = 0;
		return GF_OK;
	}

	if (brand) *brand = movie->brand->majorBrand;
	if (minorVersion) *minorVersion = movie->brand->minorVersion;
	if (AlternateBrandsCount) *AlternateBrandsCount = movie->brand->altCount;
	return GF_OK;
}