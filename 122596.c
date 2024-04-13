njs_vmcode_regexp(njs_vm_t *vm, u_char *pc)
{
    njs_regexp_t         *regexp;
    njs_vmcode_regexp_t  *code;

    code = (njs_vmcode_regexp_t *) pc;

    regexp = njs_regexp_alloc(vm, code->pattern);

    if (njs_fast_path(regexp != NULL)) {
        njs_set_regexp(&vm->retval, regexp);

        return sizeof(njs_vmcode_regexp_t);
    }

    return NJS_ERROR;
}