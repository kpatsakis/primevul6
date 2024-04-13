njs_string_prototype_from_bytes(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t unused)
{
    u_char             *p, *s, *start, *end;
    size_t             size;
    njs_int_t          ret;
    njs_slice_prop_t   slice;
    njs_string_prop_t  string;

    njs_deprecated(vm, "String.prototype.fromBytes()");

    ret = njs_string_object_validate(vm, njs_argument(args, 0));
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_string_slice_prop(vm, &string, &slice, args, nargs);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    if (string.length != 0) {
        /* ASCII or UTF8 string. */
        return njs_string_slice(vm, &vm->retval, &string, &slice);
    }

    size = 0;
    string.start += slice.start;
    end = string.start + slice.length;

    for (p = string.start; p < end; p++) {
        size += (*p < 0x80) ? 1 : 2;
    }

    start = njs_string_alloc(vm, &vm->retval, size, slice.length);

    if (njs_fast_path(start != NULL)) {

        if (size == slice.length) {
            memcpy(start, string.start, size);

        } else {
            s = start;
            end = string.start + slice.length;

            for (p = string.start; p < end; p++) {
                s = njs_utf8_encode(s, *p);
            }
        }

        return NJS_OK;
    }

    return NJS_ERROR;
}