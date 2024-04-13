njs_vmcode_try_end(njs_vm_t *vm, njs_value_t *invld, njs_value_t *offset)
{
    njs_frame_t      *frame;
    njs_exception_t  *e;

    frame = (njs_frame_t *) vm->top_frame;
    e = frame->exception.next;

    if (e == NULL) {
        frame->exception.catch = NULL;

    } else {
        frame->exception = *e;
        njs_mp_free(vm->mem_pool, e);
    }

    return (njs_jump_off_t) offset;
}