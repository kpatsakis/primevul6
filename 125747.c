njs_string_prototype_trim(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t mode)
{
    uint32_t           trim;
    njs_int_t          ret;
    njs_value_t        *value;
    njs_string_prop_t  string;

    value = njs_argument(args, 0);
    ret = njs_string_object_validate(vm, value);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    trim = njs_string_trim(value, &string, mode);

    if (trim == 0) {
        njs_value_assign(&vm->retval, value);
        return NJS_OK;
    }

    if (string.size == 0) {
        njs_value_assign(&vm->retval, &njs_string_empty);
        return NJS_OK;
    }

    return njs_string_new(vm, &vm->retval, string.start, string.size,
                          string.length);
}