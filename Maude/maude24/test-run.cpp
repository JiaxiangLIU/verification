#include <iostream>
#include <stdlib.h>

using namespace std;

int main() {
  int TESTNUM = 1;
  int TIME = 1;
  string command;

  for (int N = 5; N <= 20; N++) {
    cout << "task number " << N << " start:" << endl;
  for (int i = 0; i < TESTNUM; i++) {
    for (int j = 0; j < TIME; j++) {
      command = "./maude.linux64 real-time-maude.maude RMS.maude pow-ideal/"
	+ to_string(N) + "-" + to_string(i) + ".maude > pow-ideal-result/"
	+ to_string(N) + "-" + to_string(i) + "-" + to_string(j) + ".result";
      system(command.c_str());
      cout << "Task number " << N << " test " << i << " round " << j << " done" << endl;
    }
    cout << "Task number " << N << " test " << i << " is done!" << endl;
  }

  cout << "Task number " << N << " done!" << endl << endl;
  }

  return 0;
}
