 */
static void php_wddx_push_element(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	st_entry ent;
	wddx_stack *stack = (wddx_stack *)user_data;

	if (!strcmp(name, EL_PACKET)) {
		int i;

		if (atts) for (i=0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_VERSION)) {
				/* nothing for now */
			}
		}
	} else if (!strcmp(name, EL_STRING)) {
		ent.type = ST_STRING;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_STRING;
		Z_STRVAL_P(ent.data) = STR_EMPTY_ALLOC();
		Z_STRLEN_P(ent.data) = 0;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_BINARY)) {
		ent.type = ST_BINARY;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_STRING;
		Z_STRVAL_P(ent.data) = STR_EMPTY_ALLOC();
		Z_STRLEN_P(ent.data) = 0;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_CHAR)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_CHAR_CODE) && atts[i+1] && atts[i+1][0]) {
				char tmp_buf[2];

				snprintf(tmp_buf, sizeof(tmp_buf), "%c", (char)strtol(atts[i+1], NULL, 16));
				php_wddx_process_data(user_data, tmp_buf, strlen(tmp_buf));
				break;
			}
		}
	} else if (!strcmp(name, EL_NUMBER)) {
		ent.type = ST_NUMBER;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_LONG;
		Z_LVAL_P(ent.data) = 0;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_BOOLEAN)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_VALUE) && atts[i+1] && atts[i+1][0]) {
				ent.type = ST_BOOLEAN;
				SET_STACK_VARNAME;

				ALLOC_ZVAL(ent.data);
				INIT_PZVAL(ent.data);
				Z_TYPE_P(ent.data) = IS_BOOL;
				wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
				php_wddx_process_data(user_data, atts[i+1], strlen(atts[i+1]));
				break;
			}
		} else {
			ent.type = ST_BOOLEAN;
			SET_STACK_VARNAME;
			ZVAL_FALSE(&ent.data);
			wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
		}
	} else if (!strcmp(name, EL_NULL)) {
		ent.type = ST_NULL;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		ZVAL_NULL(ent.data);

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_ARRAY)) {
		ent.type = ST_ARRAY;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		array_init(ent.data);
		INIT_PZVAL(ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_STRUCT)) {
		ent.type = ST_STRUCT;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		array_init(ent.data);
		INIT_PZVAL(ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_VAR)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_NAME) && atts[i+1] && atts[i+1][0]) {
				if (stack->varname) efree(stack->varname);
				stack->varname = estrdup(atts[i+1]);
				break;
			}
		}
	} else if (!strcmp(name, EL_RECORDSET)) {
		int i;

		ent.type = ST_RECORDSET;
		SET_STACK_VARNAME;
		MAKE_STD_ZVAL(ent.data);
		array_init(ent.data);

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], "fieldNames") && atts[i+1] && atts[i+1][0]) {
				zval *tmp;
				char *key;
				char *p1, *p2, *endp;

				i++;
				endp = (char *)atts[i] + strlen(atts[i]);
				p1 = (char *)atts[i];
				while ((p2 = php_memnstr(p1, ",", sizeof(",")-1, endp)) != NULL) {
					key = estrndup(p1, p2 - p1);
					MAKE_STD_ZVAL(tmp);
					array_init(tmp);
					add_assoc_zval_ex(ent.data, key, p2 - p1 + 1, tmp);
					p1 = p2 + sizeof(",")-1;
					efree(key);
				}

				if (p1 <= endp) {
					MAKE_STD_ZVAL(tmp);
					array_init(tmp);
					add_assoc_zval_ex(ent.data, p1, endp - p1 + 1, tmp);
				}

				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_FIELD)) {
		int i;
		st_entry ent;

		ent.type = ST_FIELD;
		ent.varname = NULL;
		ent.data = NULL;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_NAME) && atts[i+1] && atts[i+1][0]) {
				st_entry *recordset;
				zval **field;

				if (wddx_stack_top(stack, (void**)&recordset) == SUCCESS &&
					recordset->type == ST_RECORDSET &&
					zend_hash_find(Z_ARRVAL_P(recordset->data), (char*)atts[i+1], strlen(atts[i+1])+1, (void**)&field) == SUCCESS) {
					ent.data = *field;
				}

				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_DATETIME)) {
		ent.type = ST_DATETIME;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_LONG;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	}