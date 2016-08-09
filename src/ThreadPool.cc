#include "./ThreadPool.h"

#include <pthread.h>

#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

namespace gls {

////////////////////////////////////////////////////////////////////////////////
// Job is a helper class for ThreadPool.
// Construct it by passing a pthreads-style function pointer with a pthreads
// style void poither to argumets.

ThreadPool::Job::
Job(thread_fn_p_t thread_fn_p, void *args_p)
  : thread_fn_p_(thread_fn_p), args_p_(args_p) {
  assert(thread_fn_p);
  assert(args_p);
}

ThreadPool::Job::
~Job() {
}

ThreadPool::Job::
Job(const Job &rhs) {
  this->copy(rhs);
}

ThreadPool::Job &
ThreadPool::Job::
operator=(const Job &rhs) {
  return this->copy(rhs);
}

ThreadPool::Job &
ThreadPool::Job::
copy(const Job &rhs) {
  if (&rhs == this) {
    return *this;
  }
  thread_fn_p_ = rhs.thread_fn_p_;
  args_p_ = rhs.args_p_;
  return *this;
}


////////////////////////////////////////////////////////////////////////////////
// ThreadPool is a way to start a predefined number of worker threads.
//
// These threads sit idle until sumbmit_job() is called, which provides them
// with pthread-style function pointer and argument pointer pairs (each
// encapsulated as a Job).
//
// The worker threads will keep running jobs as they become available or sit
// idle when there are none.
//
// When finish() is called, worker threads continue until all jobs have
// finished, then the worker threads join back to the main thread.
//
// It is a logic error to sumbit any jobs after finish() has been called.


// this helper function  allows c-style calling of a worker thread (which is a
// member function) and provides the worker thread access to it's ThreadPool.
void *ThreadPool_helper(void *pool_p) {
  ThreadPool &tp = *reinterpret_cast<ThreadPool *>(pool_p);

  return tp.pool_worker_fn();
}

ThreadPool::
ThreadPool(size_t num_threads)
  : thread_ids_(num_threads), jobs_(),
    finish_requested_(false), something_to_do_(false), finished_(false) {
  rcc(pthread_mutex_init(&jobs_mutex_, NULL));
  rcc(pthread_cond_init(&check_for_work_, NULL));

  // start the pool's threads
  for (size_t i = 0; i < thread_ids_.size(); ++i) {
    rcc(pthread_create(&thread_ids_[i],
                       NULL,
                       ThreadPool_helper,
                       reinterpret_cast<void *>(this)));
  }
}

ThreadPool::
~ThreadPool() {
  if (not finish_requested_) {
    finish();
  }

  rcc(pthread_cond_destroy(&check_for_work_));
  rcc(pthread_mutex_destroy(&jobs_mutex_));
}

void *
ThreadPool::
pool_worker_fn() {
  ScopedLock lock(&jobs_mutex_);
  while (true) {
    while (not something_to_do_) {
      // wait for something to do (either a job, or quit)
      rcc(pthread_cond_wait(&check_for_work_, &jobs_mutex_));
    }

    // if no jobs, then the thing to do is quit.
    if (jobs_.empty()) {
      break;
    }

    // grab job at front of the queue while mutex is locked
    Job job = jobs_.front();
    jobs_.pop();
    something_to_do_ = jobs_.size() or finish_requested_;

    // temporarily unlock mutex for the actual run
    {
      ScopedUnlock unlock(&jobs_mutex_);
      void *result_p = job.run();
      (void) result_p;  // suppress "unused variable" warning.
    }
  }
  return NULL;
}

void
ThreadPool::
submit_job(thread_fn_p_t thread_fn_p, void *args_p) {
  ScopedLock lock(&jobs_mutex_);
  jobs_.push(Job(thread_fn_p, args_p));
  something_to_do_ = true;
  rcc(pthread_cond_signal(&check_for_work_));
}

void
ThreadPool::
finish() {
  if (finished_) {
    return;
  }

  // tell everyone it's quittin' time.
  {
    ScopedLock lock(&jobs_mutex_);
    finish_requested_ = true;
    something_to_do_ = true;
    rcc(pthread_cond_broadcast(&check_for_work_));
  }

  // join up all the pool's worker threads
  for (size_t i = 0; i < thread_ids_.size(); ++i) {
    rcc(pthread_join(thread_ids_[i], NULL));
  }

  // all threads have joined
  finished_ = true;
}

}  // end namespace gls

