static void swizzle_io_read(struct MACH0_(obj_t) *obj, RIO *io) {
	r_return_if_fail (io && io->desc && io->desc->plugin);
	RIOPlugin *plugin = io->desc->plugin;
	obj->original_io_read = plugin->read;
	plugin->read = &rebasing_and_stripping_io_read;
}