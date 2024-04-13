njs_string_bytes_from(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    njs_value_t  *value;

    njs_deprecated(vm, "String.bytesFrom()");

    value = njs_arg(args, nargs, 1);

    if (njs_is_string(value)) {
        return njs_string_bytes_from_string(vm, value, njs_arg(args, nargs, 2));

    } else if (njs_is_object(value)) {

        if (njs_is_object_string(value)) {
            value = njs_object_value(value);
            return njs_string_bytes_from_string(vm, value,
                                                njs_arg(args, nargs, 2));
        }

        return njs_string_bytes_from_array_like(vm, value);
    }

    njs_type_error(vm, "value must be a string or array-like object");

    return NJS_ERROR;
}