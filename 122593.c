njs_vmcode_property_in(njs_vm_t *vm, njs_value_t *value, njs_value_t *key)
{
    njs_int_t             ret;
    njs_property_query_t  pq;

    if (njs_slow_path(njs_is_primitive(value))) {
        njs_type_error(vm, "property \"in\" on primitive %s type",
                       njs_type_string(value->type));
        return NJS_ERROR;
    }

    if (njs_slow_path(!njs_is_key(key))) {
        ret = njs_value_to_key(vm, key, key);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }
    }

    njs_property_query_init(&pq, NJS_PROPERTY_QUERY_GET, 0);

    ret = njs_property_query(vm, &pq, value, key);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    njs_set_boolean(&vm->retval, ret == NJS_OK);

    return sizeof(njs_vmcode_3addr_t);
}