#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <math.h>

using namespace std;

unsigned long long lcm(unsigned long long a, unsigned long long b) {
    unsigned long long m = a, n = b;  
    /* a, b不相等，大数减小数，直到相等为止。*/    
    while (a != b) {   
        if (a > b)  a = a - b;        
        else b = b - a;
    }
   return (m * n / a);
}

int main() {
  /*
    int N = 5;
    int TESTNUM = 50;
    int caseNum = 0;
  */

  //  int N = 10;
  int N;
  int TESTNUM = 10;
  int caseNum = 0;


  for (N = 5; N <= 20; N++) { 
    cout << "Task number is: " << N << endl;
    
    srand((unsigned)time(NULL));

    for (caseNum = 0; caseNum < TESTNUM; caseNum++ ) {

      
    int periods[N];
    for(int i = 0; i < N; i++) {
      //        periods[i] = 1 + (rand() % 100);
      periods[i] = pow(2, i);
    }
    
    
    int i,j,temp;
    for (i = 0 ; i < N; i++) {
        for (j = 0; j < N - 1 - i; j++) {
            if (periods[j] > periods[j+1]) {
                temp = periods[j];
                periods[j] = periods[j+1];
                periods[j+1] = temp;
            }
        }
    }
    
    
    unsigned long long macro = 1;
    for (i = 0; i < N; i++) {
        macro = lcm(macro, periods[i]);
    }
    cout << "round: " << caseNum << endl;
    cout << macro << endl;

    int computations[N];
    for(int i = 0; i < N; i++) {
        computations[i] = 1 + (rand() % (5000 * periods[i] / N));
    }
    
    for(int i = 0; i < N; i++) {
      cout << "period: " << periods[i] * 5 << ", computation: " << computations[i] << endl;
    }


    // write to the file

    //    ofstream out(to_string(N) + "/test-case-" + to_string(caseNum) + ".maude");
    ofstream out("pow-ideal/" + to_string(N) + "-" + to_string(caseNum) + ".maude");
    if (out.is_open()) {
      out << "set trace off ." << endl;
      out << "set break off ." << endl;
      out << "set profile off ." << endl;
      out << endl;

      out << "(tomod INSTANTIATION is " << endl;
      out << "  protecting POSRAT-TIME-DOMAIN-WITH-INF ." << endl;
      out << "  including INTERRUPT-SOURCE ." << endl;
      out << "  including PERIODIC-TASK ." << endl;
      out << "  including SYSTEM ." << endl;
      out << "  including RMS-VERIFICATION ." << endl;
      out << "  including RMS-MODEL-CHECK ." << endl;
      out << "  protecting QID ." << endl;
      out << "  subsort Qid < Oid ." << endl;
      out << endl;
      
      out << "  op tisrc : -> Object ." << endl;
      out << "  eq tisrc = < 'tisrc : IntSrc | val : 0 , cycle : 5000 > ." << endl;
      out << endl;

      out << "  op tsts : -> SysTasks ." << endl;
      //      out << "  eq tsts = [ < scheduling : Task | cnt : [ 0 / [ 38 , 38 ] ] > < switching : Task | cnt : [ 0 / [ 20 , 20 ] ] > ] ." << endl;
      out << "  eq tsts = [ < scheduling : Task | cnt : [ 0 / [ 0 , 0 ] ] > < switching : Task | cnt : [ 0 / [ 0 , 0 ] ] > ] ." << endl;      
      out << endl;
 
      out << "  op tHW : -> Hardware ." << endl;
      out << "  eq tHW = [ < 'tr : Regs | pc : none , mask : false , ir : false , temp : none > ; bottom ] ." << endl;
      out << endl;

      out << "  op tl : -> TaskList ." << endl;
      string tasks[N];
      for (i = 0; i < N; i++) {
	tasks[i] = "< 't" + to_string(i+1) + " : PTask | priority : " + to_string(N-i) + " , period : " + to_string(periods[i])
	  //	  + " , status : DORMANT , cnt : [ 0 / [ " + to_string(computations[i]*1000) + " , " + to_string(computations[i]*1000) + " ] ] >";
	  + " , status : DORMANT , cnt : [ 0 / [ " + to_string(computations[i]) + " , " + to_string(computations[i]) + " ] ] >";
      }
      out << "  eq tl = ";
      for (i = 0; i < N; i++) {
	out << tasks[i] << endl;
	out << "          :: ";
      }
      out << "null ." << endl;
      out << endl;

      out << "  op tinit : -> GlobalSystem ." << endl;
      out << "  eq tinit = { tl [ 0 / " + to_string(macro) + " ] tsts tHW tisrc } ." << endl;
      out << endl;

      out << "endtom)" << endl;
      out << endl;

      out << "(set tick max .)" << endl;
      out << "(mc tinit |=u []schedulable .)" << endl;
      out << "quit" << endl;
      
      out.close();
    }

    }

  }
    return 0;
}
