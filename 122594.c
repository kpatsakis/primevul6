njs_vmcode_proto_init(njs_vm_t *vm, njs_value_t *value, njs_value_t *unused,
    njs_value_t *init)
{
    njs_object_t        *obj;
    njs_jump_off_t      ret;
    njs_object_prop_t   *prop;
    njs_lvlhsh_query_t  lhq;

    lhq.key = njs_str_value("__proto__");
    lhq.key_hash = NJS___PROTO___HASH;
    lhq.proto = &njs_object_hash_proto;
    lhq.pool = vm->mem_pool;

    obj = njs_object(value);

    ret = njs_lvlhsh_find(&obj->__proto__->shared_hash, &lhq);
    if (njs_slow_path(ret != NJS_OK)) {
        goto fail;
    }

    prop = lhq.value;

    if (prop->type != NJS_PROPERTY_HANDLER) {
        goto fail;
    }

    ret = prop->value.data.u.prop_handler(vm, prop, value, init, &vm->retval);
    if (njs_slow_path(ret != NJS_OK)) {
        goto fail;
    }

    return sizeof(njs_vmcode_prop_set_t);

fail:

    njs_internal_error(vm, "\"__proto__\" init failed");
    return NJS_ERROR;
}