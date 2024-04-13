njs_vmcode_error(njs_vm_t *vm, u_char *pc)
{
    njs_vmcode_error_t  *err;

    err = (njs_vmcode_error_t *) pc;

    if (err->type == NJS_OBJ_TYPE_REF_ERROR) {
        njs_reference_error(vm, "\"%V\" is not defined", &err->u.name);

    } else {
        njs_error_fmt_new(vm, &vm->retval, err->type, "%V", &err->u.message);
    }
}