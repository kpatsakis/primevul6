njs_string_atob(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    size_t        i, n, len, pad;
    u_char        *dst, *tmp, *p;
    ssize_t       size;
    njs_str_t     str;
    njs_int_t     ret;
    njs_chb_t     chain;
    njs_value_t   *value, lvalue;
    const u_char  *b64, *s;

    value = njs_lvalue_arg(&lvalue, args, nargs, 1);

    ret = njs_value_to_string(vm, value, value);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    /* Forgiving-base64 decode. */

    b64 = njs_basis64;
    njs_string_get(value, &str);

    tmp = njs_mp_alloc(vm->mem_pool, str.length);
    if (tmp == NULL) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    p = tmp;

    for (i = 0; i < str.length; i++) {
        if (njs_slow_path(str.start[i] == ' ')) {
            continue;
        }

        *p++ = str.start[i];
    }

    pad = 0;
    str.start = tmp;
    str.length = p - tmp;

    if (str.length % 4 == 0) {
        if (str.length > 0) {
            if (str.start[str.length - 1] == '=') {
                pad += 1;
            }

            if (str.start[str.length - 2] == '=') {
                pad += 1;
            }
        }

    } else if (str.length % 4 == 1) {
        goto error;
    }

    for (i = 0; i < str.length - pad; i++) {
        if (njs_slow_path(b64[str.start[i]] == 77)) {
            goto error;
        }
    }

    len = njs_base64_decoded_length(str.length, pad);

    njs_chb_init(&chain, vm->mem_pool);

    dst = njs_chb_reserve(&chain, len * 2);
    if (njs_slow_path(dst == NULL)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    n = len;
    s = str.start;

    while (n >= 3) {
        njs_chb_write_byte_as_utf8(&chain, b64[s[0]] << 2 | b64[s[1]] >> 4);
        njs_chb_write_byte_as_utf8(&chain, b64[s[1]] << 4 | b64[s[2]] >> 2);
        njs_chb_write_byte_as_utf8(&chain, b64[s[2]] << 6 | b64[s[3]]);

        s += 4;
        n -= 3;
    }

    if (n >= 1) {
        njs_chb_write_byte_as_utf8(&chain, b64[s[0]] << 2 | b64[s[1]] >> 4);
    }

    if (n >= 2) {
        njs_chb_write_byte_as_utf8(&chain, b64[s[1]] << 4 | b64[s[2]] >> 2);
    }

    size = njs_chb_size(&chain);
    if (njs_slow_path(size < 0)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    if (size == 0) {
        njs_value_assign(&vm->retval, &njs_string_empty);
        return NJS_OK;
    }

    dst = njs_string_alloc(vm, &vm->retval, size, len);
    if (njs_slow_path(dst == NULL)) {
        return NJS_ERROR;
    }

    njs_chb_join_to(&chain, dst);
    njs_chb_destroy(&chain);

    njs_mp_free(vm->mem_pool, tmp);

    return NJS_OK;

error:

    njs_type_error(vm, "the string to be decoded is not correctly encoded");

    return NJS_ERROR;
}