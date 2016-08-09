#ifndef SCOPEDLOCK_H_
#define SCOPEDLOCK_H_

#include "./Threads.h"

namespace gls {

// to lock with guaranteed unlocking
class ScopedLock {
  public:
    explicit ScopedLock(pthread_mutex_t *mutex_p)
      : mutex_p_(mutex_p) {
      rcc(pthread_mutex_lock(mutex_p_));
    }

    ~ScopedLock() {
      rcc(pthread_mutex_unlock(mutex_p_));
    }

  private:
    ScopedLock();  // don't define: no uninitialized instances.
    ScopedLock(const ScopedLock &rhs);  // don't define: no copies.
    ScopedLock &operator=(const ScopedLock &rhs);  // don't define: no copies.

  private:
    pthread_mutex_t *mutex_p_;
};

// to unlock with guaranteed re-locking.
class ScopedUnlock {
  public:
    explicit ScopedUnlock(pthread_mutex_t *mutex_p)
      : mutex_p_(mutex_p) {
      rcc(pthread_mutex_unlock(mutex_p_));
    }

    ~ScopedUnlock() {
      rcc(pthread_mutex_lock(mutex_p_));
    }

  private:
    ScopedUnlock();  // don't define: no uninitialized instances.
    explicit ScopedUnlock(const ScopedLock &rhs);  // don't define: no copies.
    ScopedUnlock &operator=(const ScopedLock &rhs);  // don't define: no copies.

  private:
    pthread_mutex_t *mutex_p_;
};

////////////////////////////////////////////////////////////////////////////////

class ScopedReadLock {
  public:
    explicit ScopedReadLock(pthread_rwlock_t *rwlock_p)
      : rwlock_p_(rwlock_p) {
      rcc(pthread_rwlock_rdlock(rwlock_p_));
    }

    ~ScopedReadLock() {
      rcc(pthread_rwlock_unlock(rwlock_p_));
    }

  private:
    // don't define: no uninitialized instances.
    ScopedReadLock();

    // don't define: no copies.
    ScopedReadLock(const ScopedReadLock &rhs);

    // don't define: no copies.
    ScopedReadLock &operator=(const ScopedReadLock &rhs);

  private:
    pthread_rwlock_t *rwlock_p_;
};


class ScopedWriteLock {
  public:
    explicit ScopedWriteLock(pthread_rwlock_t *rwlock_p)
      : rwlock_p_(rwlock_p) {
      rcc(pthread_rwlock_wrlock(rwlock_p_));
    }

    ~ScopedWriteLock() {
      rcc(pthread_rwlock_unlock(rwlock_p_));
    }

  private:
    // don't define: no uninitialized instances.
    ScopedWriteLock();

    // don't define: no copies.
    ScopedWriteLock(const ScopedWriteLock &rhs);

    // don't define: no copies.
    ScopedWriteLock &operator=(const ScopedWriteLock &rhs);

  private:
    pthread_rwlock_t *rwlock_p_;
};

}  // end namespace gls

#endif  // SCOPEDLOCK_H_
