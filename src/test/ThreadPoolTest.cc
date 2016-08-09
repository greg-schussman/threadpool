#include <gtest/gtest.h>

#include "../ThreadPool.h"

using gls::ThreadPool;
using std::cout;
using std::endl;
using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////

// this is data for the thread to operate on.
// it can store (or point to) both inputs and results.
struct Data {
  public:
    Data(int id) : id_(id), result_(0), data_p_(NULL) { }
    ~Data() { }

    Data(const Data &rhs) {
      id_ = rhs.id_;
      result_ = rhs.result_;
      data_p_ = rhs.data_p_;
    }

    void run() {
      result_ = 10 * id_;
      data_p_ = this;
    }

  public:
    int id_;
    int result_;
    Data *data_p_;

  private:
    Data();  // don't define
};

// this is the thread that operates on the data
void *thread_func(void *void_data_p) {
  Data *data_p = reinterpret_cast<Data *>(void_data_p);
  Data &data = *data_p;

  data.id_ = 7;
  data.result_ = 42;
  data.data_p_ = reinterpret_cast<Data *>(void_data_p);

  return data.data_p_;
}

////////////////////////////////////////////////////////////////////////////////

TEST(ThreadPoolTest, thread_fn_direct) {
  Data d(1);
  void *arg_p = reinterpret_cast<void *>(&d);

  EXPECT_NE(7, d.id_);
  EXPECT_NE(42, d.result_);
  EXPECT_EQ(NULL, d.data_p_);

  void *result = thread_func(arg_p);
  EXPECT_EQ(arg_p, result);

  EXPECT_EQ(7, d.id_);
  EXPECT_EQ(42, d.result_);
  EXPECT_EQ(arg_p, d.data_p_);
}

TEST(ThreadPoolTest, thread_fn_indirect) {
  Data d(1);
  void *arg_p = reinterpret_cast<void *>(&d);

  EXPECT_NE(7, d.id_);
  EXPECT_NE(42, d.result_);
  EXPECT_EQ(NULL, d.data_p_);

  ThreadPool::thread_fn_p_t f_p = thread_func;
  void *result_p = f_p(arg_p);
  EXPECT_EQ(arg_p, result_p);

  EXPECT_EQ(7, d.id_);
  EXPECT_EQ(42, d.result_);
  EXPECT_EQ(arg_p, d.data_p_);
}

TEST(ThreadPoolTest, Job) {
  Data d(1);
  void *arg_p = reinterpret_cast<void *>(&d);

  ThreadPool::Job j(thread_func, arg_p);
  EXPECT_TRUE(thread_func == j.thread_fn_p_);
  EXPECT_EQ(arg_p, j.args_p_);

  void *result_p = j.run();
  EXPECT_EQ(arg_p, result_p);

  EXPECT_EQ(7, d.id_);
  EXPECT_EQ(42, d.result_);
  EXPECT_EQ(arg_p, d.data_p_);
}

TEST(ThreadPoolTest, submit_job_c_style_single) {
  Data d(1);
  void *arg_p = reinterpret_cast<void *>(&d);

  // submit all jobs for scheduling
  ThreadPool tp(1);

  tp.submit_job(thread_func, &d);

  // wait for everything to finish
  tp.finish();

  // test the results
  EXPECT_EQ(7, d.id_);
  EXPECT_EQ(42, d.result_);
  EXPECT_EQ(arg_p, d.data_p_);
}


TEST(ThreadPoolTest, submit_job_c_style_multi) {
  const size_t kNumJobs = 1000;
  vector<Data> objs;
  objs.reserve(kNumJobs);

  for (size_t i = 0; i < kNumJobs; ++i) {
    objs.push_back(Data(1));
  }

  ThreadPool tp(16);

  // submit all jobs for scheduling
  for (size_t i = 0; i < kNumJobs; ++i) {
    void *arg_p = reinterpret_cast<void *>(&objs[i]);
    tp.submit_job(thread_func, arg_p);
  }

  // wait for everything to finish
  tp.finish();

  // test the results
  for (size_t i = 0; i < kNumJobs; ++i) {
    EXPECT_EQ(7, objs[i].id_);
    EXPECT_EQ(42, objs[i].result_);
    EXPECT_EQ(&objs[i], objs[i].data_p_);
  }
}



