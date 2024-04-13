njs_vmcode_instance_of(njs_vm_t *vm, njs_value_t *object,
    njs_value_t *constructor)
{
    njs_value_t        value, bound;
    njs_object_t       *prototype, *proto;
    njs_function_t     *function;
    njs_jump_off_t     ret;
    const njs_value_t  *retval;

    static const njs_value_t prototype_string = njs_string("prototype");

    if (!njs_is_function(constructor)) {
        njs_type_error(vm, "right argument is not callable");
        return NJS_ERROR;
    }

    function = njs_function(constructor);

    if (function->bound != NULL) {
        function = function->u.bound_target;
        njs_set_function(&bound, function);
        constructor = &bound;
    }

    retval = &njs_value_false;

    if (njs_is_object(object)) {
        ret = njs_value_property(vm, constructor,
                                 njs_value_arg(&prototype_string), &value);

        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        if (njs_fast_path(ret == NJS_OK)) {
            if (njs_slow_path(!njs_is_object(&value))) {
                njs_type_error(vm, "Function has non-object prototype "
                               "in instanceof");
                return NJS_ERROR;
            }

            prototype = njs_object(&value);
            proto = njs_object(object);

            do {
                proto = proto->__proto__;

                if (proto == prototype) {
                    retval = &njs_value_true;
                    break;
                }

            } while (proto != NULL);
        }
    }

    vm->retval = *retval;

    return sizeof(njs_vmcode_instance_of_t);
}