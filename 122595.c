njs_vmcode_property_foreach(njs_vm_t *vm, njs_value_t *object,
    njs_value_t *invld, u_char *pc)
{
    njs_property_next_t        *next;
    njs_vmcode_prop_foreach_t  *code;

    next = njs_mp_alloc(vm->mem_pool, sizeof(njs_property_next_t));
    if (njs_slow_path(next == NULL)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    next->index = 0;
    next->array = njs_value_enumerate(vm, object, NJS_ENUM_KEYS,
                                      NJS_ENUM_STRING, 0);
    if (njs_slow_path(next->array == NULL)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    vm->retval.data.u.next = next;

    code = (njs_vmcode_prop_foreach_t *) pc;

    return code->offset;
}