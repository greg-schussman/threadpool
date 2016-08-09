#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <pthread.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

#include "./Threads.h"
#include "./ScopedLock.h"

namespace gls {

// allows us access to the ThreadPool from within ThreadPool's worker threads.
void *ThreadPool_helper(void *pool_p);


class ThreadPool {
  public:
    typedef void *(*thread_fn_p_t)(void *v_p);

    // encapsulate a Job, which is just a function pointer and corresponding
    // argument pointer.
    class Job {
      public:
        Job(thread_fn_p_t thread_fn_p, void *args_p);
        ~Job();

        // copying
        Job(const Job &rhs);
        Job &operator=(const Job &rhs);
        Job &copy(const Job &rhs);

        void *run() { return thread_fn_p_(args_p_); }

      public:
        thread_fn_p_t thread_fn_p_;
        void *args_p_;
    };

  public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    // can't be called directly, so called indirectly via ThreadPool_helper
    // function.
    void *pool_worker_fn();

    void submit_job(thread_fn_p_t thread_fn_p, void *args_p);

    // block until all submitted jobs have finished and the ThreadPool's worker
    // threads have joined.  It is a logic error to submit a job after calling
    // firish();
    void finish();

  private:
    std::vector<pthread_t> thread_ids_;
    std::queue<Job> jobs_;  // boolean predicate: jobs_.size()
    pthread_cond_t check_for_work_;  // new work exists, or it's quitting time.
    pthread_mutex_t jobs_mutex_;
    bool finish_requested_;  // tell worker threads to quit
    bool something_to_do_;   // condition variable predicate.
    bool finished_;          // all threads have joined
};

}  // end namespace gls

#endif  // THREADPOOL_H_

