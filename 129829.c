compare_interface_impl (const void *a, const void *b)
{
	const guint32 *a_values = a;
	const guint32 *b_values = b;

	int klass = a_values [MONO_INTERFACEIMPL_CLASS] - b_values [MONO_INTERFACEIMPL_CLASS];
	if (klass)
		return klass;

	return a_values [MONO_INTERFACEIMPL_INTERFACE] - b_values [MONO_INTERFACEIMPL_INTERFACE];
}