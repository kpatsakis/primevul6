static int r_cmd_java_handle_insert_method_ref (RCore *core, const char *input) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *bin = anal ? (RBinJavaObj *) r_cmd_java_get_bin_obj (anal) : NULL;
	const char *p = input? r_cmd_java_consumetok (input, ' ', -1): NULL, *n = NULL;
	char  *classname=NULL, *name = NULL, *descriptor = NULL;
	ut32 cn_sz = 0, n_sz = 0, d_sz = 0;
	int res = false;

	if (!bin) {
		return res;
	}
	if (!anal || !anal->fcns || r_list_length (anal->fcns) == 0) {
		eprintf ("Unable to access the current analysis, perform 'af' for function analysis.\n");
		return true;
	}
	if (!p) {
		return res;
	}

	n = p && *p ? r_cmd_java_strtok (p, ' ', -1) : NULL;
	classname = n && p && p != n ? malloc (n-p+1) : NULL;
	cn_sz = n && p ? n-p +1 : 0;
	if (!classname) {
		return res;
	}

	snprintf (classname, cn_sz, "%s", p);
	//memset (classname, 0, cn_sz);
	//memcpy (classname, p, n-p);
	p = n+1;
	n = p && *p ? r_cmd_java_strtok (p, ' ', -1) : NULL;
	name = n && p && p != n ? malloc (n-p+1) : NULL;
	n_sz = n && p ? n-p +1 : 0;
	if (!name) {
		free (classname);
		return res;
	}
	snprintf (name, n_sz, "%s", p);
	//memset (name, 0, n_sz);
	//memcpy (name, p, n-p);

	p = n + 1;
	n =  p && *p ? r_cmd_java_strtok (p, ' ', -1) : NULL;
	if (n) {
		descriptor = n && p && p != n ? malloc (n-p+1) : NULL;
		d_sz = n-p +1;
	} else if (p && *p) {
		d_sz = strlen (p) + 1;
		descriptor = d_sz > 1 ? malloc (d_sz) : NULL;
	}

	if (!descriptor) {
		free (classname);
		free (name);
		return res;
	}
	snprintf (descriptor, d_sz, "%s", p);
	//memset (descriptor, 0, d_sz);
	//memcpy (descriptor, p, n-p);

	r_cons_printf ("Would be adding class name:%s, name: %s, descriptor: %s\n", classname, name, descriptor);
	free (classname);
	free (name);
	free (descriptor);
	res = true;
	return res;
}