static gboolean
verify_interfaces (MonoClass *class)
{
	int i;
	for (i = 0; i < class->interface_count; ++i) {
		MonoClass *iface = class->interfaces [i];
		if (!(iface->flags & TYPE_ATTRIBUTE_INTERFACE))
			return FALSE;
	}
	return TRUE;