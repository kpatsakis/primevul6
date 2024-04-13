assembly_add_win32_resources (MonoDynamicImage *assembly, MonoReflectionAssemblyBuilder *assemblyb)
{
	char *buf;
	char *p;
	guint32 size, i;
	MonoReflectionWin32Resource *win32_res;
	ResTreeNode *tree;

	if (!assemblyb->win32_resources)
		return;

	/*
	 * Resources are stored in a three level tree inside the PE file.
	 * - level one contains a node for each type of resource
	 * - level two contains a node for each resource
	 * - level three contains a node for each instance of a resource for a
	 *   specific language.
	 */

	tree = resource_tree_create (assemblyb->win32_resources);

	/* Estimate the size of the encoded tree */
	size = 0;
	for (i = 0; i < mono_array_length (assemblyb->win32_resources); ++i) {
		win32_res = (MonoReflectionWin32Resource*)mono_array_addr (assemblyb->win32_resources, MonoReflectionWin32Resource, i);
		size += mono_array_length (win32_res->res_data);
	}
	/* Directory structure */
	size += mono_array_length (assemblyb->win32_resources) * 256;
	p = buf = g_malloc (size);

	resource_tree_encode (tree, p, p, &p);

	g_assert (p - buf <= size);

	assembly->win32_res = g_malloc (p - buf);
	assembly->win32_res_size = p - buf;
	memcpy (assembly->win32_res, buf, p - buf);

	g_free (buf);
	resource_tree_free (tree);
}