#include "./Threads.h"

#include <pthread.h>
#include <errno.h>

#include <iostream>

using std::cout;
using std::flush;

namespace gls {

// return code check for pthread calls
void rcc(int i) {
  if (false) {  // set true for debugging
    cout << "#### rcc: " << i << "(";
    switch (i) {
      case 0:
        cout << "OK";
        break;
      case EAGAIN:
        cout << "EAGAIN";
        break;
      case EINVAL:
        cout << "EINVAL";
        break;
      case EPERM:
        cout << "EPERM";
        break;
      default:
        cout << "unknown ???";
    }
    cout << ")" << flush;
  }
}

}  // end namespace gls
