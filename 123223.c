static RBinJavaObj * r_cmd_java_get_bin_obj(RAnal *anal) {
	RBin *b;
	int is_java;
	RBinPlugin *plugin;
	if (!anal || !anal->binb.bin) {
		return NULL;
	}
	b = anal->binb.bin;
	if (!b->cur || !b->cur->o) {
		return NULL;
	}
	plugin = b->cur->o->plugin;
	is_java = (plugin  && strcmp (plugin->name, "java") == 0) ? 1 : 0;
	return is_java ? b->cur->o->bin_obj : NULL;
}