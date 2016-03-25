#include <iostream>
#include <stdlib.h>

using namespace std;

int main() {
  system("./maude.linux64 real-time-maude.maude RMS.maude case.maude > test1.log");
  cout << "good" << endl;
  return 0;
}
