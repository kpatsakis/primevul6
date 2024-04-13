njs_vmcode_finally(njs_vm_t *vm, njs_value_t *invld, njs_value_t *retval,
    u_char *pc)
{
    njs_value_t           *exception_value, *exit_value;
    njs_vmcode_finally_t  *finally;

    exception_value = njs_scope_value(vm, (njs_index_t) retval);

    if (njs_is_valid(exception_value)) {
        vm->retval = *exception_value;

        return NJS_ERROR;
    }

    finally = (njs_vmcode_finally_t *) pc;

    exit_value = njs_scope_value(vm, finally->exit_value);

    /*
     * exit_value is set by:
     *   vmcode_try_start to INVALID 0
     *   vmcode_try_break to INVALID 1
     *   vmcode_try_continue to INVALID -1
     *   vmcode_try_return to a valid return value
     */

    if (njs_is_valid(exit_value)) {
        return njs_vmcode_return(vm, NULL, exit_value);

    } else if (njs_number(exit_value) != 0) {
        return (njs_jump_off_t) (njs_number(exit_value) > 0)
                                ? finally->break_offset
                                : finally->continue_offset;
    }

    return sizeof(njs_vmcode_finally_t);
}