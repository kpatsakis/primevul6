md_build_attr_append_substr(MD_CTX* ctx, MD_ATTRIBUTE_BUILD* build,
                            MD_TEXTTYPE type, OFF off)
{
    if(build->substr_count >= build->substr_alloc) {
        MD_TEXTTYPE* new_substr_types;
        OFF* new_substr_offsets;

        build->substr_alloc = (build->substr_alloc > 0
                ? build->substr_alloc + build->substr_alloc / 2
                : 8);
        new_substr_types = (MD_TEXTTYPE*) realloc(build->substr_types,
                                    build->substr_alloc * sizeof(MD_TEXTTYPE));
        if(new_substr_types == NULL) {
            MD_LOG("realloc() failed.");
            return -1;
        }
        /* Note +1 to reserve space for final offset (== raw_size). */
        new_substr_offsets = (OFF*) realloc(build->substr_offsets,
                                    (build->substr_alloc+1) * sizeof(OFF));
        if(new_substr_offsets == NULL) {
            MD_LOG("realloc() failed.");
            free(new_substr_types);
            return -1;
        }

        build->substr_types = new_substr_types;
        build->substr_offsets = new_substr_offsets;
    }

    build->substr_types[build->substr_count] = type;
    build->substr_offsets[build->substr_count] = off;
    build->substr_count++;
    return 0;
}