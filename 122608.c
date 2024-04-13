njs_vmcode_template_literal(njs_vm_t *vm, njs_value_t *invld1,
    njs_value_t *retval)
{
    njs_array_t     *array;
    njs_value_t     *value;
    njs_jump_off_t  ret;

    static const njs_function_t  concat = {
          .native = 1,
          .args_offset = 1,
          .u.native = njs_string_prototype_concat
    };

    value = njs_scope_valid_value(vm, (njs_index_t) retval);

    if (!njs_is_primitive(value)) {
        array = njs_array(value);

        ret = njs_function_frame(vm, (njs_function_t *) &concat,
                                 &njs_string_empty, array->start,
                                 array->length, 0);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }

        ret = njs_function_frame_invoke(vm, value);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }
    }

    return sizeof(njs_vmcode_template_literal_t);
}