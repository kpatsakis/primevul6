njs_vmcode_try_start(njs_vm_t *vm, njs_value_t *exception_value,
    njs_value_t *offset, u_char *pc)
{
    njs_value_t             *exit_value;
    njs_frame_t             *frame;
    njs_exception_t         *e;
    njs_vmcode_try_start_t  *try_start;

    frame = (njs_frame_t *) vm->top_frame;

    if (frame->exception.catch != NULL) {
        e = njs_mp_alloc(vm->mem_pool, sizeof(njs_exception_t));
        if (njs_slow_path(e == NULL)) {
            njs_memory_error(vm);
            return NJS_ERROR;
        }

        *e = frame->exception;
        frame->exception.next = e;
    }

    frame->exception.catch = pc + (njs_jump_off_t) offset;

    njs_set_invalid(exception_value);

    try_start = (njs_vmcode_try_start_t *) pc;
    exit_value = njs_scope_value(vm, try_start->exit_value);

    njs_set_invalid(exit_value);
    njs_number(exit_value) = 0;

    return sizeof(njs_vmcode_try_start_t);
}