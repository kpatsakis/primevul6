static ut8 _(r_cmd_java_obj_ref)(const char *name, const char *class_name, ut32 len) {
	if (!name || !class_name) {
		return false;
	}
	if (strncmp (class_name, name, len)) {
		return false;
	}
	if (*(name - 1) == 'L' && *(name + len) == ';') {
		return true;
	}
	if (!strncmp (class_name, name, len) && !*(name + len)) {
		return true;
	}
	return false;
}