  wrap(ProxyMutex *mutex,             ///< Mutex for this continuation (primary lock).
       Continuation *target,          ///< "Real" continuation we want to call.
       int eventId = EVENT_IMMEDIATE, ///< Event ID for invocation of @a target.
       void *edata = nullptr          ///< Data for invocation of @a target.
  )
  {
    EThread *eth = this_ethread();
    if (!target->mutex) {
      // If there's no mutex, plugin doesn't care about locking so why should we?
      target->handleEvent(eventId, edata);
    } else {
      MUTEX_TRY_LOCK(lock, target->mutex, eth);
      if (lock.is_locked()) {
        target->handleEvent(eventId, edata);
      } else {
        eventProcessor.schedule_imm(new ContWrapper(mutex, target, eventId, edata), ET_NET);
      }
    }
  }