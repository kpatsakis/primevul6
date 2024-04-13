njs_function_new_object(njs_vm_t *vm, njs_value_t *constructor)
{
    njs_value_t     proto, bound;
    njs_object_t    *object;
    njs_function_t  *function;
    njs_jump_off_t  ret;

    const njs_value_t prototype_string = njs_string("prototype");

    object = njs_object_alloc(vm);
    if (njs_slow_path(object == NULL)) {
        return NULL;
    }

    function = njs_function(constructor);

    if (function->bound != NULL) {
        function = function->u.bound_target;
        njs_set_function(&bound, function);
        constructor = &bound;
    }

    ret = njs_value_property(vm, constructor, njs_value_arg(&prototype_string),
                             &proto);

    if (njs_slow_path(ret == NJS_ERROR)) {
        return NULL;
    }

    if (njs_fast_path(njs_is_object(&proto))) {
        object->__proto__ = njs_object(&proto);
    }

    return object;
}