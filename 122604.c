njs_vmcode_arguments(njs_vm_t *vm, u_char *pc)
{
    njs_frame_t             *frame;
    njs_value_t             *value;
    njs_jump_off_t          ret;
    njs_vmcode_arguments_t  *code;

    frame = (njs_frame_t *) vm->active_frame;

    if (frame->native.arguments_object == NULL) {
        ret = njs_function_arguments_object_init(vm, &frame->native);
        if (njs_slow_path(ret != NJS_OK)) {
            return NJS_ERROR;
        }
    }

    code = (njs_vmcode_arguments_t *) pc;

    value = njs_scope_valid_value(vm, code->dst);
    if (njs_slow_path(value == NULL)) {
        return NJS_ERROR;
    }

    njs_set_object(value, frame->native.arguments_object);

    return sizeof(njs_vmcode_arguments_t);
}