gs_font_map_glyph_by_dict(const gs_memory_t *mem, const ref *map, gs_glyph glyph, ushort *u, unsigned int length)
{
    ref *v, n;
    uchar *unicode_return = (uchar *)u;
    if (glyph >= GS_MIN_CID_GLYPH) {
        uint cid = glyph - GS_MIN_CID_GLYPH;

        if (dict_find_string(map, "CIDCount", &v) > 0) {
            /* This is a CIDDEcoding resource. */
            make_int(&n, cid / 256);
            if (dict_find(map, &n, &v) > 0) {
                ref vv;

                if (array_get(mem, v, cid % 256, &vv) == 0 && r_type(&vv) == t_integer) {
                    if (v->value.intval > 65535) {
                        if (length < 4)
                            return 4;
                        unicode_return[0] = v->value.intval >> 24;
                        unicode_return[1] = (v->value.intval & 0x00FF0000) >> 16;
                        unicode_return[2] = (v->value.intval & 0x0000FF00) >> 8;
                        unicode_return[3] = v->value.intval & 0xFF;
                        return 4;
                    } else {
                        if (length < 2)
                            return 2;
                        unicode_return[0] = v->value.intval >> 8;
                        unicode_return[1] = v->value.intval & 0xFF;
                        return 2;
                    }
                }
            }
            return 0; /* Absent in the map. */
        }
        /* This is GlyphNames2Unicode dictionary. */
        make_int(&n, cid);
    } else
        name_index_ref(mem, glyph, &n);
     if (dict_find(map, &n, &v) > 0) {
        if (r_has_type(v, t_string)) {
            int l = r_size(v);

            if (length < l)
                return l;
            memcpy(unicode_return, v->value.const_bytes, l);
            return l;
        }
        if (r_type(v) == t_integer) {
            if (v->value.intval > 65535) {
                if (length < 4)
                    return 4;
                unicode_return[0] = v->value.intval >> 24;
                unicode_return[1] = (v->value.intval & 0x00FF0000) >> 16;
                unicode_return[2] = (v->value.intval & 0x0000FF00) >> 8;
                unicode_return[3] = v->value.intval & 0xFF;
                return 4;
            } else {
                if (length < 2)
                    return 2;
                unicode_return[0] = v->value.intval >> 8;
                unicode_return[1] = v->value.intval & 0xFF;
                return 2;
            }
        }
    }
    return 0; /* Absent in the map. */
}