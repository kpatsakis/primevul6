njs_string_prototype_to_utf8(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    njs_int_t          ret;
    njs_slice_prop_t   slice;
    njs_string_prop_t  string;

    njs_deprecated(vm, "String.prototype.toUTF8()");

    ret = njs_string_object_validate(vm, njs_argument(args, 0));
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    (void) njs_string_prop(&string, njs_argument(args, 0));

    string.length = 0;
    slice.string_length = string.size;

    ret = njs_string_slice_args(vm, &slice, args, nargs);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    return njs_string_slice(vm, &vm->retval, &string, &slice);
}