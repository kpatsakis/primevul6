njs_string_prototype_to_bytes(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    u_char                *p;
    size_t                length;
    uint32_t              byte;
    njs_int_t             ret;
    const u_char          *s, *end;
    njs_slice_prop_t      slice;
    njs_string_prop_t     string;
    njs_unicode_decode_t  ctx;

    njs_deprecated(vm, "String.prototype.toBytes()");

    ret = njs_string_object_validate(vm, njs_argument(args, 0));
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_string_slice_prop(vm, &string, &slice, args, nargs);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    if (string.length == 0) {
        /* Byte string. */
        return njs_string_slice(vm, &vm->retval, &string, &slice);
    }

    p = njs_string_alloc(vm, &vm->retval, slice.length, 0);

    if (njs_fast_path(p != NULL)) {

        if (string.length != string.size) {
            /* UTF-8 string. */
            end = string.start + string.size;

            s = njs_string_offset(string.start, end, slice.start);

            length = slice.length;

            njs_utf8_decode_init(&ctx);

            while (length != 0 && s < end) {
                byte = njs_utf8_decode(&ctx, &s, end);

                if (njs_slow_path(byte > 0xFF)) {
                    njs_release(vm, &vm->retval);
                    vm->retval = njs_value_null;

                    return NJS_OK;
                }

                *p++ = (u_char) byte;
                length--;
            }

        } else {
            /* ASCII string. */
            memcpy(p, string.start + slice.start, slice.length);
        }

        return NJS_OK;
    }

    return NJS_ERROR;
}