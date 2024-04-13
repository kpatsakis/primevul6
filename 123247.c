static int r_cmd_java_handle_find_cp_const (RCore *core, const char *cmd) {
	const char *p = (cmd && *cmd == ' ')?  r_cmd_java_consumetok (cmd, ' ', -1): NULL;
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (get_anal (core));
	RAnalFunction *fcn = NULL;
	RAnalBlock *bb = NULL;
	RListIter *bb_iter, *fn_iter, *iter;
	RCmdJavaCPResult *cp_res = NULL;
	ut16 idx = -1;
	RList *find_list;

	if (p && *p == 'a') {
		idx = -1;
	} else {
		idx = r_cmd_java_get_input_num_value (core, p);
	}

	IFDBG r_cons_printf ("Function call made: %s\n", cmd);

	if (!obj) {
		eprintf ("[-] r_cmd_java: no valid java bins found.\n");
		return true;
	}
	if (!cmd || !*cmd) {
		eprintf ("[-] r_cmd_java: invalid command syntax.\n");
		r_cmd_java_print_cmd_help (JAVA_CMDS+FIND_CP_CONST_IDX);
		return true;
	}
	if (idx == 0) {
		eprintf ("[-] r_cmd_java: invalid CP Obj Index Supplied.\n");
		return true;
	}
	find_list = r_list_new ();
	find_list->free = free;
	// XXX - this will break once RAnal moves to sdb
	r_list_foreach (core->anal->fcns, fn_iter, fcn) {
		r_list_foreach (fcn->bbs, bb_iter, bb) {
			char op = bb->op_bytes[0];
			cp_res = NULL;
			switch (op) {
			case 0x12:
				cp_res = (idx == (ut16) -1) || (bb->op_bytes[1] == idx) ?
							R_NEW0(RCmdJavaCPResult) : NULL;
				if (cp_res) {
					cp_res->idx = bb->op_bytes[1];
				}
				break;
			case 0x13:
			case 0x14:
				cp_res = (idx == (ut16) -1) || (R_BIN_JAVA_USHORT (bb->op_bytes, 1) == idx) ?
							R_NEW0(RCmdJavaCPResult) : NULL;
				if (cp_res) {
					cp_res->idx = R_BIN_JAVA_USHORT (bb->op_bytes, 1);
				}
				break;
			}
			if (cp_res) {
				cp_res->addr = bb->addr;
				cp_res->obj = r_bin_java_get_item_from_cp (obj, cp_res->idx);
				r_list_append (find_list, cp_res);
			}
		}
	}
	if (idx == (ut16) -1) {
		r_list_foreach (find_list, iter, cp_res) {
			const char *t = ((RBinJavaCPTypeMetas *) cp_res->obj->metas->type_info)->name;
			r_cons_printf ("@0x%"PFMT64x" idx = %d Type = %s\n", cp_res->addr, cp_res->idx, t);
		}

	} else {
		r_list_foreach (find_list, iter, cp_res) {
			r_cons_printf ("@0x%"PFMT64x"\n", cp_res->addr);
		}
	}
	r_list_free (find_list);
	return true;
}