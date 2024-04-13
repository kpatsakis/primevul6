njs_vmcode_debug(njs_vm_t *vm, u_char *pc, const char *prefix)
{
    njs_vm_code_t  *code;

    code = njs_lookup_code(vm, pc);

    njs_printf("%s %V\n", prefix,
               (code != NULL) ? &code->name : &njs_entry_unknown);
}