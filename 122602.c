njs_function_frame_create(njs_vm_t *vm, njs_value_t *value,
    const njs_value_t *this, uintptr_t nargs, njs_bool_t ctor)
{
    njs_value_t     val;
    njs_object_t    *object;
    njs_function_t  *function;

    if (njs_fast_path(njs_is_function(value))) {

        function = njs_function(value);

        if (ctor) {
            if (!function->ctor) {
                njs_type_error(vm, "%s is not a constructor",
                               njs_type_string(value->type));
                return NJS_ERROR;
            }

            if (!function->native) {
                object = njs_function_new_object(vm, value);
                if (njs_slow_path(object == NULL)) {
                    return NJS_ERROR;
                }

                njs_set_object(&val, object);
                this = &val;
            }
        }

        return njs_function_frame(vm, function, this, NULL, nargs, ctor);
    }

    njs_type_error(vm, "%s is not a function", njs_type_string(value->type));

    return NJS_ERROR;
}