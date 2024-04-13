md_build_attribute(MD_CTX* ctx, const CHAR* raw_text, SZ raw_size,
                   unsigned flags, MD_ATTRIBUTE* attr, MD_ATTRIBUTE_BUILD* build)
{
    OFF raw_off, off;
    int is_trivial;
    int ret = 0;

    memset(build, 0, sizeof(MD_ATTRIBUTE_BUILD));

    /* If there is no backslash and no ampersand, build trivial attribute
     * without any malloc(). */
    is_trivial = TRUE;
    for(raw_off = 0; raw_off < raw_size; raw_off++) {
        if(ISANYOF3_(raw_text[raw_off], _T('\\'), _T('&'), _T('\0'))) {
            is_trivial = FALSE;
            break;
        }
    }

    if(is_trivial) {
        build->text = (CHAR*) (raw_size ? raw_text : NULL);
        build->substr_types = build->trivial_types;
        build->substr_offsets = build->trivial_offsets;
        build->substr_count = 1;
        build->substr_alloc = 0;
        build->trivial_types[0] = MD_TEXT_NORMAL;
        build->trivial_offsets[0] = 0;
        build->trivial_offsets[1] = raw_size;
        off = raw_size;
    } else {
        build->text = (CHAR*) malloc(raw_size * sizeof(CHAR));
        if(build->text == NULL) {
            MD_LOG("malloc() failed.");
            goto abort;
        }

        raw_off = 0;
        off = 0;

        while(raw_off < raw_size) {
            if(raw_text[raw_off] == _T('\0')) {
                MD_CHECK(md_build_attr_append_substr(ctx, build, MD_TEXT_NULLCHAR, off));
                memcpy(build->text + off, raw_text + raw_off, 1);
                off++;
                raw_off++;
                continue;
            }

            if(raw_text[raw_off] == _T('&')) {
                OFF ent_end;

                if(md_is_entity_str(ctx, raw_text, raw_off, raw_size, &ent_end)) {
                    MD_CHECK(md_build_attr_append_substr(ctx, build, MD_TEXT_ENTITY, off));
                    memcpy(build->text + off, raw_text + raw_off, ent_end - raw_off);
                    off += ent_end - raw_off;
                    raw_off = ent_end;
                    continue;
                }
            }

            if(build->substr_count == 0  ||  build->substr_types[build->substr_count-1] != MD_TEXT_NORMAL)
                MD_CHECK(md_build_attr_append_substr(ctx, build, MD_TEXT_NORMAL, off));

            if(!(flags & MD_BUILD_ATTR_NO_ESCAPES)  &&
               raw_text[raw_off] == _T('\\')  &&  raw_off+1 < raw_size  &&
               (ISPUNCT_(raw_text[raw_off+1]) || ISNEWLINE_(raw_text[raw_off+1])))
                raw_off++;

            build->text[off++] = raw_text[raw_off++];
        }
        build->substr_offsets[build->substr_count] = off;
    }

    attr->text = build->text;
    attr->size = off;
    attr->substr_offsets = build->substr_offsets;
    attr->substr_types = build->substr_types;
    return 0;

abort:
    md_free_attribute(ctx, build);
    return -1;
}