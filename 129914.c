mono_custom_attrs_from_builders (MonoImage *alloc_img, MonoImage *image, MonoArray *cattrs)
{
	int i, index, count, not_visible;
	MonoCustomAttrInfo *ainfo;
	MonoReflectionCustomAttr *cattr;

	if (!cattrs)
		return NULL;
	/* FIXME: check in assembly the Run flag is set */

	count = mono_array_length (cattrs);

	/* Skip nonpublic attributes since MS.NET seems to do the same */
	/* FIXME: This needs to be done more globally */
	not_visible = 0;
	for (i = 0; i < count; ++i) {
		cattr = (MonoReflectionCustomAttr*)mono_array_get (cattrs, gpointer, i);
		if (!custom_attr_visible (image, cattr))
			not_visible ++;
	}
	count -= not_visible;

	ainfo = image_g_malloc0 (alloc_img, MONO_SIZEOF_CUSTOM_ATTR_INFO + sizeof (MonoCustomAttrEntry) * count);

	ainfo->image = image;
	ainfo->num_attrs = count;
	ainfo->cached = alloc_img != NULL;
	index = 0;
	for (i = 0; i < count; ++i) {
		cattr = (MonoReflectionCustomAttr*)mono_array_get (cattrs, gpointer, i);
		if (custom_attr_visible (image, cattr)) {
			unsigned char *saved = mono_image_alloc (image, mono_array_length (cattr->data));
			memcpy (saved, mono_array_addr (cattr->data, char, 0), mono_array_length (cattr->data));
			ainfo->attrs [index].ctor = cattr->ctor->method;
			ainfo->attrs [index].data = saved;
			ainfo->attrs [index].data_size = mono_array_length (cattr->data);
			index ++;
		}
	}

	return ainfo;
}