njs_vmcode_function(njs_vm_t *vm, u_char *pc)
{
    njs_function_t         *function;
    njs_vmcode_function_t  *code;
    njs_function_lambda_t  *lambda;

    code = (njs_vmcode_function_t *) pc;
    lambda = code->lambda;

    function = njs_function_alloc(vm, lambda, code->async);
    if (njs_slow_path(function == NULL)) {
        return NJS_ERROR;
    }

    if (njs_function_capture_closure(vm, function, lambda) != NJS_OK) {
        return NJS_ERROR;
    }

    function->args_count = lambda->nargs - lambda->rest_parameters;

    njs_set_function(&vm->retval, function);

    return sizeof(njs_vmcode_function_t);
}