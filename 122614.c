njs_vmcode_object(njs_vm_t *vm)
{
    njs_object_t  *object;

    object = njs_object_alloc(vm);

    if (njs_fast_path(object != NULL)) {
        njs_set_object(&vm->retval, object);

        return sizeof(njs_vmcode_object_t);
    }

    return NJS_ERROR;
}