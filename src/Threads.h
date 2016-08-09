#ifndef THREADS_H_
#define THREADS_H_

#include <sys/syscall.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

namespace gls {

// get thread's process id.
inline pid_t gettid() { return syscall(__NR_gettid); }

inline bool is_main_thread() { return getpid() == gettid(); }

// return code check for pthread calls
void rcc(int i);

// for when horrible things (non-threadsafe code called from non-parent thread,
// NULL pointer when obj should exist, etc.)
inline void panic(const std::string &message) {
  using std::cerr;
  using std::cout;
  using std::endl;
  using std::ofstream;
  using std::string;

  string msg = "panic: " + message;
  cout << msg << endl;
  cerr << msg << endl;
  ofstream ofs("panic.log");
  ofs << msg << endl;
  ofs.close();
  abort();
}

}  // end namespace gls

#endif  // THREADS_H_
