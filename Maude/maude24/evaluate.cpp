#include <iostream>
#include <fstream>
#include <string>

using namespace std;
int main() {
  ifstream filein("test1.log");

  string pre2Line = "", preLine = "", curLine = "";
  bool found = false; // the "untimed model check" line is found
  int firstPos, lastPos;
  int lineNum = 0;
  string timeStr;
  unsigned long time;
  bool modelCheckResult;
  for ( ; getline(filein, curLine); ) {
    if (found) {
      lineNum++;
      if (lineNum == 3) {
	if (curLine.size() < 25) modelCheckResult = true;
	else modelCheckResult = false;
	break;
      }
    } else if (curLine.size() >= 7 && curLine[0] == 'U' && curLine[1] == 'n') {
      found = true;
      lineNum = 0;
      firstPos = pre2Line.find('(') + 1;
      lastPos = pre2Line.find("ms real", firstPos);
      timeStr = pre2Line.substr(firstPos, lastPos - firstPos);
      time = stoul(timeStr);
    } else {
      pre2Line = preLine;
      preLine = curLine;
    }
  }
  cout << "result: " << (modelCheckResult ? "true" : "false") << endl;
  cout << "time: " << time << endl;
  return 0;
}
