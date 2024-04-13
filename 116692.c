static int add_info_ref(const char *path, const unsigned char *sha1, int flag, void *cb_data)
{
	struct object *o = parse_object(sha1);
	if (!o)
		return -1;

	fprintf(info_ref_fp, "%s	%s\n", sha1_to_hex(sha1), path);
	if (o->type == OBJ_TAG) {
		o = deref_tag(o, path, 0);
		if (o)
			fprintf(info_ref_fp, "%s	%s^{}\n",
				sha1_to_hex(o->sha1), path);
	}
	return 0;
}