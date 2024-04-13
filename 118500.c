static void gf_m2ts_metadata_pointer_descriptor_del(GF_M2TS_MetadataPointerDescriptor *metapd)
{
	if (metapd) {
		if (metapd->locator_data) gf_free(metapd->locator_data);
		if (metapd->data) gf_free(metapd->data);
		gf_free(metapd);
	}
}