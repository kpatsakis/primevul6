njs_string_prototype_from_utf8(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t unused)
{
    ssize_t            length;
    njs_int_t          ret;
    njs_slice_prop_t   slice;
    njs_string_prop_t  string;

    njs_deprecated(vm, "String.prototype.fromUTF8()");

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

    string.start += slice.start;

    length = njs_utf8_length(string.start, slice.length);

    if (length >= 0) {
        return njs_string_new(vm, &vm->retval, string.start, slice.length,
                              length);
    }

    vm->retval = njs_value_null;

    return NJS_OK;
}