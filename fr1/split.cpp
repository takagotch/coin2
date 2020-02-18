
//
#include <vector>
#include <string>

using namespace std;

vector<string> split_naive(const string&s, char delim) {
  vector<string> elems;
  string item;
  for (char ch: s) {
    if (ch == delim) {
      if (!item.empty())
        elems.push_back(item);
      item.clear();
    }
  }
  if (!item.empty())
    elems.push_back(item);

  return elems;
}

// benchmark test1
void bench1() {
  string input = "The introduction of splitter method";
  char delim = ' ';

  cout << "split: " << measeure(split, input, delim, 100000) / (CLOCKS_PER_SEC / 1000) << "ms" << endl;
  cout << "naive: " << measeure(split_naive, input, delim, 100000) / (CLOCKS_PER_SEC / 1000) << "ms" << endl;
}

// benchmark test2
void bench2() {
  string random_input = "";
  int len = 1000000;
  mt19937 engine;
  uniform_int_distribution<int> distribution(0, 25);
  char delim = distribution(engine) + 'a';
  for (int p=0; p<len; ++p) {
    int ch = distribution(engine);
  random_input += ch + 'a';
  }

  cout << "split: " << measeure(split, random_input, delim, 10) / (CLOCKS_PER_SEC / 1000) << "ms" << endl;
  cout << "naive: " << measeure(split_naive, random_input, delim, 10) / (CLOCKS_PER_SEC / 1000) << "ms" << endl;
}

// stringstream
#include <vector>
#include <string>
#include <sstream>

using namespace std;

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
  if (!item.empty()) {
    elems.push_back(item);
  }
  return elems;
}


