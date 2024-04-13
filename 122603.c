njs_vmcode_try_continue(njs_vm_t *vm, njs_value_t *exit_value,
    njs_value_t *offset)
{
    njs_number(exit_value) = -1;

    return (njs_jump_off_t) offset;
}