

#ifndef _TEST_H_
#define _TEST_H_

#include "Vose.h"


class test {

public:
  /** Test imple Vose insertion. */
  bool test1();
  /** Test accuracy of Vose implementation */
  bool test2();
  /** Test VoseTree insertion, moving from bags to marbles, accuracy */
  bool test3();
  /** Test VoseTree bag compression */
  bool test4();

};


#endif // _TEST_H_
