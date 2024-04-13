void ciEnv::register_method(ciMethod* target,
                            int entry_bci,
                            CodeOffsets* offsets,
                            int orig_pc_offset,
                            CodeBuffer* code_buffer,
                            int frame_words,
                            OopMapSet* oop_map_set,
                            ExceptionHandlerTable* handler_table,
                            ImplicitExceptionTable* inc_table,
                            AbstractCompiler* compiler,
                            int comp_level,
                            bool has_unsafe_access,
                            bool has_wide_vectors,
                            RTMState  rtm_state) {
  VM_ENTRY_MARK;
  nmethod* nm = NULL;
  {
    // To prevent compile queue updates.
    MutexLocker locker(MethodCompileQueue_lock, THREAD);

    // Prevent SystemDictionary::add_to_hierarchy from running
    // and invalidating our dependencies until we install this method.
    // No safepoints are allowed. Otherwise, class redefinition can occur in between.
    MutexLocker ml(Compile_lock);
    No_Safepoint_Verifier nsv;

    // Change in Jvmti state may invalidate compilation.
    if (!failing() && jvmti_state_changed()) {
      record_failure("Jvmti state change invalidated dependencies");
    }

    // Change in DTrace flags may invalidate compilation.
    if (!failing() &&
        ( (!dtrace_extended_probes() && ExtendedDTraceProbes) ||
          (!dtrace_method_probes() && DTraceMethodProbes) ||
          (!dtrace_alloc_probes() && DTraceAllocProbes) )) {
      record_failure("DTrace flags change invalidated dependencies");
    }

    if (!failing()) {
      if (log() != NULL) {
        // Log the dependencies which this compilation declares.
        dependencies()->log_all_dependencies();
      }

      // Encode the dependencies now, so we can check them right away.
      dependencies()->encode_content_bytes();

      // Check for {class loads, evolution, breakpoints, ...} during compilation
      validate_compile_task_dependencies(target);
    }

    methodHandle method(THREAD, target->get_Method());

#if INCLUDE_RTM_OPT
    if (!failing() && (rtm_state != NoRTM) &&
        (method()->method_data() != NULL) &&
        (method()->method_data()->rtm_state() != rtm_state)) {
      // Preemptive decompile if rtm state was changed.
      record_failure("RTM state change invalidated rtm code");
    }
#endif

    if (failing()) {
      // While not a true deoptimization, it is a preemptive decompile.
      MethodData* mdo = method()->method_data();
      if (mdo != NULL) {
        mdo->inc_decompile_count();
      }

      // All buffers in the CodeBuffer are allocated in the CodeCache.
      // If the code buffer is created on each compile attempt
      // as in C2, then it must be freed.
      code_buffer->free_blob();
      return;
    }

    assert(offsets->value(CodeOffsets::Deopt) != -1, "must have deopt entry");
    assert(offsets->value(CodeOffsets::Exceptions) != -1, "must have exception entry");

    nm =  nmethod::new_nmethod(method,
                               compile_id(),
                               entry_bci,
                               offsets,
                               orig_pc_offset,
                               debug_info(), dependencies(), code_buffer,
                               frame_words, oop_map_set,
                               handler_table, inc_table,
                               compiler, comp_level);
    // Free codeBlobs
    code_buffer->free_blob();

    if (nm != NULL) {
      nm->set_has_unsafe_access(has_unsafe_access);
      nm->set_has_wide_vectors(has_wide_vectors);
#if INCLUDE_RTM_OPT
      nm->set_rtm_state(rtm_state);
#endif

      // Record successful registration.
      // (Put nm into the task handle *before* publishing to the Java heap.)
      if (task() != NULL) {
        task()->set_code(nm);
      }

      if (entry_bci == InvocationEntryBci) {
        if (TieredCompilation) {
          // If there is an old version we're done with it
          nmethod* old = method->code();
          if (TraceMethodReplacement && old != NULL) {
            ResourceMark rm;
            char *method_name = method->name_and_sig_as_C_string();
            tty->print_cr("Replacing method %s", method_name);
          }
          if (old != NULL) {
            old->make_not_entrant();
          }
        }
        if (TraceNMethodInstalls) {
          ResourceMark rm;
          char *method_name = method->name_and_sig_as_C_string();
          ttyLocker ttyl;
          tty->print_cr("Installing method (%d) %s ",
                        comp_level,
                        method_name);
        }
        // Allow the code to be executed
        method->set_code(method, nm);
      } else {
        if (TraceNMethodInstalls) {
          ResourceMark rm;
          char *method_name = method->name_and_sig_as_C_string();
          ttyLocker ttyl;
          tty->print_cr("Installing osr method (%d) %s @ %d",
                        comp_level,
                        method_name,
                        entry_bci);
        }
        method->method_holder()->add_osr_nmethod(nm);
      }
    }
  }  // safepoints are allowed again

  if (nm != NULL) {
    // JVMTI -- compiled method notification (must be done outside lock)
    nm->post_compiled_method_load_event();
  } else {
    // The CodeCache is full. Print out warning and disable compilation.
    record_failure("code cache is full");
    CompileBroker::handle_full_code_cache();
  }
}