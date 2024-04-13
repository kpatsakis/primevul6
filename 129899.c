custom_attr_visible (MonoImage *image, MonoReflectionCustomAttr *cattr)
{
	/* FIXME: Need to do more checks */
	if (cattr->ctor->method && (cattr->ctor->method->klass->image != image)) {
		int visibility = cattr->ctor->method->klass->flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;

		if ((visibility != TYPE_ATTRIBUTE_PUBLIC) && (visibility != TYPE_ATTRIBUTE_NESTED_PUBLIC))
			return FALSE;
	}

	return TRUE;
}