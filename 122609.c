njs_vmcode_function_copy(njs_vm_t *vm, njs_value_t *value, njs_index_t retidx)
{
    njs_value_t     *retval;
    njs_function_t  *function;

    retval = njs_scope_valid_value(vm, retidx);

    if (njs_is_undefined(retval)) {
        *retval = *value;

        function = njs_function_value_copy(vm, retval);
        if (njs_slow_path(function == NULL)) {
            return NJS_ERROR;
        }
    }

    return sizeof(njs_vmcode_function_copy_t);
}