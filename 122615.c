njs_vmcode_property_init(njs_vm_t *vm, njs_value_t *value, njs_value_t *key,
    njs_value_t *init)
{
    double              num;
    uint32_t            index, size;
    njs_int_t           ret;
    njs_array_t         *array;
    njs_value_t         *val, name;
    njs_object_prop_t   *prop;
    njs_lvlhsh_query_t  lhq;

    switch (value->type) {
    case NJS_ARRAY:
        num = njs_key_to_index(key);
        if (njs_slow_path(!njs_key_is_integer_index(num, key))) {
            njs_internal_error(vm,
                               "invalid index while property initialization");
            return NJS_ERROR;
        }

        index = (uint32_t) num;
        array = value->data.u.array;

        if (index >= array->length) {
            size = index - array->length;

            ret = njs_array_expand(vm, array, 0, size + 1);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }

            val = &array->start[array->length];

            while (size != 0) {
                njs_set_invalid(val);
                val++;
                size--;
            }

            array->length = index + 1;
        }

        /* GC: retain. */
        array->start[index] = *init;

        break;

    case NJS_OBJECT:
        ret = njs_value_to_key(vm, &name, key);
        if (njs_slow_path(ret != NJS_OK)) {
            return NJS_ERROR;
        }

        njs_object_property_key_set(&lhq, &name, 0);
        lhq.proto = &njs_object_hash_proto;
        lhq.pool = vm->mem_pool;

        prop = njs_object_prop_alloc(vm, &name, init, 1);
        if (njs_slow_path(prop == NULL)) {
            return NJS_ERROR;
        }

        lhq.value = prop;
        lhq.replace = 1;

        ret = njs_lvlhsh_insert(njs_object_hash(value), &lhq);
        if (njs_slow_path(ret != NJS_OK)) {
            njs_internal_error(vm, "lvlhsh insert/replace failed");
            return NJS_ERROR;
        }

        break;

    default:
        njs_internal_error(vm, "unexpected object type \"%s\" "
                           "while property initialization",
                           njs_type_string(value->type));

        return NJS_ERROR;
    }

    return sizeof(njs_vmcode_prop_set_t);
}