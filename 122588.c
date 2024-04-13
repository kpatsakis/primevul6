njs_vmcode_return(njs_vm_t *vm, njs_value_t *invld, njs_value_t *retval)
{
    njs_frame_t         *frame;
    njs_native_frame_t  *previous;

    frame = (njs_frame_t *) vm->top_frame;

    if (frame->native.ctor) {
        if (njs_is_object(retval)) {
            njs_release(vm, frame->native.local[0]);

        } else {
            retval = frame->native.local[0];
        }
    }

    previous = njs_function_previous_frame(&frame->native);

    njs_vm_scopes_restore(vm, &frame->native, previous);

    *frame->native.retval = *retval;

    njs_function_frame_free(vm, &frame->native);

    return NJS_OK;
}