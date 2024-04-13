compare_declsecurity_attrs (const void *a, const void *b)
{
	const guint32 *a_values = a;
	const guint32 *b_values = b;

	return a_values [MONO_DECL_SECURITY_PARENT] - b_values [MONO_DECL_SECURITY_PARENT];
}