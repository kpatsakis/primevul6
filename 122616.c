njs_vmcode_try_break(njs_vm_t *vm, njs_value_t *exit_value,
    njs_value_t *offset)
{
    /* exit_value can contain valid value set by vmcode_try_return. */
    if (!njs_is_valid(exit_value)) {
        njs_number(exit_value) = 1;
    }

    return (njs_jump_off_t) offset;
}