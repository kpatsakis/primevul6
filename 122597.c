njs_vmcode_debugger(njs_vm_t *vm)
{
    /*
     * HOW TO DEBUG JS CODE:
     * 1) put debugger instruction when certain condition is met
     *    in the JS code:
     *    function test() {
     *      if (<>) debugger;
     *    }
     * 2) set a breakpoint to njs_vmcode_debugger() in gdb.
     *
     * To see the values of certain indices:
     * 1) use njs -d test.js to dump the byte code
     * 2) find an appropriate index around DEBUGGER instruction
     * 3) in gdb: p *njs_scope_value_get(vm, <index as a hex literal>)
     **/

    njs_set_undefined(&vm->retval);

    return sizeof(njs_vmcode_debugger_t);
}