void ciEnv::validate_compile_task_dependencies(ciMethod* target) {
  if (failing())  return;  // no need for further checks

  // First, check non-klass dependencies as we might return early and
  // not check klass dependencies if the system dictionary
  // modification counter hasn't changed (see below).
  for (Dependencies::DepStream deps(dependencies()); deps.next(); ) {
    if (deps.is_klass_type())  continue;  // skip klass dependencies
    Klass* witness = deps.check_dependency();
    if (witness != NULL) {
      record_failure("invalid non-klass dependency");
      return;
    }
  }

  // Klass dependencies must be checked when the system dictionary
  // changes.  If logging is enabled all violated dependences will be
  // recorded in the log.  In debug mode check dependencies even if
  // the system dictionary hasn't changed to verify that no invalid
  // dependencies were inserted.  Any violated dependences in this
  // case are dumped to the tty.
  bool counter_changed = system_dictionary_modification_counter_changed();

  bool verify_deps = trueInDebug;
  if (!counter_changed && !verify_deps)  return;

  int klass_violations = 0;
  for (Dependencies::DepStream deps(dependencies()); deps.next(); ) {
    if (!deps.is_klass_type())  continue;  // skip non-klass dependencies
    Klass* witness = deps.check_dependency();
    if (witness != NULL) {
      klass_violations++;
      if (!counter_changed) {
        // Dependence failed but counter didn't change.  Log a message
        // describing what failed and allow the assert at the end to
        // trigger.
        deps.print_dependency(witness);
      } else if (xtty == NULL) {
        // If we're not logging then a single violation is sufficient,
        // otherwise we want to log all the dependences which were
        // violated.
        break;
      }
    }
  }

  if (klass_violations != 0) {
#ifdef ASSERT
    if (!counter_changed && !PrintCompilation) {
      // Print out the compile task that failed
      _task->print_line();
    }
#endif
    assert(counter_changed, "failed dependencies, but counter didn't change");
    record_failure("concurrent class loading");
  }
}