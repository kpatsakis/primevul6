load_cattr_value (MonoImage *image, MonoType *t, const char *p, const char **end)
{
	int slen, type = t->type;
	MonoClass *tklass = t->data.klass;

handle_enum:
	switch (type) {
	case MONO_TYPE_U1:
	case MONO_TYPE_I1:
	case MONO_TYPE_BOOLEAN: {
		MonoBoolean *bval = g_malloc (sizeof (MonoBoolean));
		*bval = *p;
		*end = p + 1;
		return bval;
	}
	case MONO_TYPE_CHAR:
	case MONO_TYPE_U2:
	case MONO_TYPE_I2: {
		guint16 *val = g_malloc (sizeof (guint16));
		*val = read16 (p);
		*end = p + 2;
		return val;
	}
#if SIZEOF_VOID_P == 4
	case MONO_TYPE_U:
	case MONO_TYPE_I:
#endif
	case MONO_TYPE_R4:
	case MONO_TYPE_U4:
	case MONO_TYPE_I4: {
		guint32 *val = g_malloc (sizeof (guint32));
		*val = read32 (p);
		*end = p + 4;
		return val;
	}
#if SIZEOF_VOID_P == 8
	case MONO_TYPE_U: /* error out instead? this should probably not happen */
	case MONO_TYPE_I:
#endif
	case MONO_TYPE_U8:
	case MONO_TYPE_I8: {
		guint64 *val = g_malloc (sizeof (guint64));
		*val = read64 (p);
		*end = p + 8;
		return val;
	}
	case MONO_TYPE_R8: {
		double *val = g_malloc (sizeof (double));
		readr8 (p, val);
		*end = p + 8;
		return val;
	}
	case MONO_TYPE_VALUETYPE:
		if (t->data.klass->enumtype) {
			type = mono_class_enum_basetype (t->data.klass)->type;
			goto handle_enum;
		} else {
			MonoClass *k =  t->data.klass;
			
			if (mono_is_corlib_image (k->image) && strcmp (k->name_space, "System") == 0 && strcmp (k->name, "DateTime") == 0){
				guint64 *val = g_malloc (sizeof (guint64));
				*val = read64 (p);
				*end = p + 8;
				return val;
			}
		}
		g_error ("generic valutype %s not handled in custom attr value decoding", t->data.klass->name);
		break;
		
	case MONO_TYPE_STRING:
		if (*p == (char)0xFF) {
			*end = p + 1;
			return NULL;
		}
		slen = mono_metadata_decode_value (p, &p);
		*end = p + slen;
		return mono_string_new_len (mono_domain_get (), p, slen);
	case MONO_TYPE_CLASS: {
		char *n;
		MonoType *t;
		if (*p == (char)0xFF) {
			*end = p + 1;
			return NULL;
		}
handle_type:
		slen = mono_metadata_decode_value (p, &p);
		n = g_memdup (p, slen + 1);
		n [slen] = 0;
		t = mono_reflection_type_from_name (n, image);
		if (!t)
			g_warning ("Cannot load type '%s'", n);
		g_free (n);
		*end = p + slen;
		if (t)
			return mono_type_get_object (mono_domain_get (), t);
		else
			return NULL;
	}
	case MONO_TYPE_OBJECT: {
		char subt = *p++;
		MonoObject *obj;
		MonoClass *subc = NULL;
		void *val;

		if (subt == 0x50) {
			goto handle_type;
		} else if (subt == 0x0E) {
			type = MONO_TYPE_STRING;
			goto handle_enum;
		} else if (subt == 0x1D) {
			MonoType simple_type = {{0}};
			int etype = *p;
			p ++;

			if (etype == 0x51)
				/* See Partition II, Appendix B3 */
				etype = MONO_TYPE_OBJECT;
			type = MONO_TYPE_SZARRAY;
			simple_type.type = etype;
			tklass = mono_class_from_mono_type (&simple_type);
			goto handle_enum;
		} else if (subt == 0x55) {
			char *n;
			MonoType *t;
			slen = mono_metadata_decode_value (p, &p);
			n = g_memdup (p, slen + 1);
			n [slen] = 0;
			t = mono_reflection_type_from_name (n, image);
			if (!t)
				g_error ("Cannot load type '%s'", n);
			g_free (n);
			p += slen;
			subc = mono_class_from_mono_type (t);
		} else if (subt >= MONO_TYPE_BOOLEAN && subt <= MONO_TYPE_R8) {
			MonoType simple_type = {{0}};
			simple_type.type = subt;
			subc = mono_class_from_mono_type (&simple_type);
		} else {
			g_error ("Unknown type 0x%02x for object type encoding in custom attr", subt);
		}
		val = load_cattr_value (image, &subc->byval_arg, p, end);
		obj = mono_object_new (mono_domain_get (), subc);
		g_assert (!subc->has_references);
		memcpy ((char*)obj + sizeof (MonoObject), val, mono_class_value_size (subc, NULL));
		g_free (val);
		return obj;
	}
	case MONO_TYPE_SZARRAY: {
		MonoArray *arr;
		guint32 i, alen, basetype;
		alen = read32 (p);
		p += 4;
		if (alen == 0xffffffff) {
			*end = p;
			return NULL;
		}
		arr = mono_array_new (mono_domain_get(), tklass, alen);
		basetype = tklass->byval_arg.type;
		if (basetype == MONO_TYPE_VALUETYPE && tklass->enumtype)
			basetype = mono_class_enum_basetype (tklass)->type;
		switch (basetype)
		{
			case MONO_TYPE_U1:
			case MONO_TYPE_I1:
			case MONO_TYPE_BOOLEAN:
				for (i = 0; i < alen; i++) {
					MonoBoolean val = *p++;
					mono_array_set (arr, MonoBoolean, i, val);
				}
				break;
			case MONO_TYPE_CHAR:
			case MONO_TYPE_U2:
			case MONO_TYPE_I2:
				for (i = 0; i < alen; i++) {
					guint16 val = read16 (p);
					mono_array_set (arr, guint16, i, val);
					p += 2;
				}
				break;
			case MONO_TYPE_R4:
			case MONO_TYPE_U4:
			case MONO_TYPE_I4:
				for (i = 0; i < alen; i++) {
					guint32 val = read32 (p);
					mono_array_set (arr, guint32, i, val);
					p += 4;
				}
				break;
			case MONO_TYPE_R8:
				for (i = 0; i < alen; i++) {
					double val;
					readr8 (p, &val);
					mono_array_set (arr, double, i, val);
					p += 8;
				}
				break;
			case MONO_TYPE_U8:
			case MONO_TYPE_I8:
				for (i = 0; i < alen; i++) {
					guint64 val = read64 (p);
					mono_array_set (arr, guint64, i, val);
					p += 8;
				}
				break;
			case MONO_TYPE_CLASS:
			case MONO_TYPE_OBJECT:
			case MONO_TYPE_STRING:
				for (i = 0; i < alen; i++) {
					MonoObject *item = load_cattr_value (image, &tklass->byval_arg, p, &p);
					mono_array_setref (arr, i, item);
				}
				break;
			default:
				g_error ("Type 0x%02x not handled in custom attr array decoding", basetype);
		}
		*end=p;
		return arr;
	}
	default:
		g_error ("Type 0x%02x not handled in custom attr value decoding", type);
	}
	return NULL;
}