njs_string_btoa(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    u_char                *dst;
    size_t                len, length;
    uint32_t              cp0, cp1, cp2;
    njs_int_t             ret;
    njs_value_t           *value, lvalue;
    const u_char          *p, *end;
    njs_string_prop_t     string;
    njs_unicode_decode_t  ctx;

    value = njs_lvalue_arg(&lvalue, args, nargs, 1);

    ret = njs_value_to_string(vm, value, value);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    len = njs_string_prop(&string, value);

    p = string.start;
    end = string.start + string.size;

    njs_utf8_decode_init(&ctx);

    length = njs_base64_encoded_length(len);

    dst = njs_string_alloc(vm, &vm->retval, length, length);
    if (njs_slow_path(dst == NULL)) {
        return NJS_ERROR;
    }

    while (len > 2 && p < end) {
        cp0 = njs_utf8_decode(&ctx, &p, end);
        cp1 = njs_utf8_decode(&ctx, &p, end);
        cp2 = njs_utf8_decode(&ctx, &p, end);

        if (njs_slow_path(cp0 > 0xff || cp1 > 0xff || cp2 > 0xff)) {
            goto error;
        }

        *dst++ = njs_basis64_enc[cp0 >> 2];
        *dst++ = njs_basis64_enc[((cp0 & 0x03) << 4) | (cp1 >> 4)];
        *dst++ = njs_basis64_enc[((cp1 & 0x0f) << 2) | (cp2 >> 6)];
        *dst++ = njs_basis64_enc[cp2 & 0x3f];

        len -= 3;
    }

    if (len > 0) {
        cp0 = njs_utf8_decode(&ctx, &p, end);
        if (njs_slow_path(cp0 > 0xff)) {
            goto error;
        }

        *dst++ = njs_basis64_enc[cp0 >> 2];

        if (len == 1) {
            *dst++ = njs_basis64_enc[(cp0 & 0x03) << 4];
            *dst++ = '=';
            *dst++ = '=';

        } else {
            cp1 = njs_utf8_decode(&ctx, &p, end);
            if (njs_slow_path(cp1 > 0xff)) {
                goto error;
            }

            *dst++ = njs_basis64_enc[((cp0 & 0x03) << 4) | (cp1 >> 4)];
            *dst++ = njs_basis64_enc[(cp1 & 0x0f) << 2];
            *dst++ = '=';
        }

    }

    return NJS_OK;

error:

    njs_type_error(vm, "invalid character (>= U+00FF)");

    return NJS_ERROR;
}