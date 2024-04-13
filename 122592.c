njs_vmcode_import(njs_vm_t *vm, njs_mod_t *module, njs_value_t *retval)
{
    njs_int_t     ret;
    njs_arr_t     *m;
    njs_value_t   *value;
    njs_object_t  *object;

    if (vm->modules == NULL) {
        vm->modules = njs_arr_create(vm->mem_pool, 4, sizeof(njs_value_t));
        if (njs_slow_path(vm->modules == NULL)) {
            njs_memory_error(vm);
            return NJS_ERROR;
        }

        m = vm->modules;

        value = njs_arr_add_multiple(m, vm->shared->module_items);
        if (njs_slow_path(value == NULL)) {
            njs_memory_error(vm);
            return NJS_ERROR;
        }

        njs_memzero(m->start, m->items * sizeof(njs_value_t));
    }

    value = njs_arr_item(vm->modules, module->index);

    if (!njs_is_null(value)) {
        njs_value_assign(retval, value);
        return sizeof(njs_vmcode_import_t);
    }

    if (module->function.native) {
        njs_value_assign(value, &module->value);

        object = njs_object_value_copy(vm, value);
        if (njs_slow_path(object == NULL)) {
            return NJS_ERROR;
        }

    } else {
        njs_set_invalid(value);
        ret = njs_vm_invoke(vm, &module->function, NULL, 0, value);
        if (ret == NJS_ERROR) {
            return ret;
        }
    }

    njs_value_assign(retval, value);

    return sizeof(njs_vmcode_import_t);
}