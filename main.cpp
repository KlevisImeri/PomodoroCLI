// clang-format off
#include <bits/stdc++.h>
#include <chrono>
#include <ostream>
using namespace std;
using namespace chrono;
using namespace this_thread;
using i64 = long long;
using str = string;
using wchar = wchar_t;
using wstr = wstring;
#define vec vector
#define arr array
#define all(x) x.begin(), x.end()
#define p(x) cout << (x) << endl
#define pv(x) for(auto& _:x){cout<<(_)<<' ';}cout<<endl;

#ifdef LOCAL
#include "debug.h"
#else
#define d(x...)
#define out(x...)
#endif

void work(); 
void rest();

const int work_min = 30;
const int rest_min = 5;
const str ui_full = "█";
const str ui_empty = "░";
const str work_sound = "resources/'A dream…I saw a dream (subaru).mp3'";
const str rest_sound = "resources/'never meant to belong.mp3'";

// You need to install mpv
// TODO: Make global thread so only one sound is run 
void play(str res) { 
  thread([res] { //thread save copy
    string command = "mpv --no-terminal " + res;
    system(command.c_str());
  }).detach();
}

void clear() {system("clear");}

vec<str> new_progbar(int size) {
  vec<str> progbar(size);
  for(auto& s:progbar) s = ui_empty;
  return progbar;
}

void print(vec<str> bar, int progres) {
  for(auto& s:bar) cout<<s;
  cout<<bar.size()-progres<<flush;
}

void loop(int goal) {
  int progres = 0;
  auto progbar = new_progbar(goal);
  print(progbar, progres);
  while(progres != goal) {
    sleep_for(minutes(1));
    progbar[progres] = ui_full;
    progres++;
    wcout<<L"\r";
    print(progbar, progres);
  }
}

void work() {
  clear();
  p("Starting work session");
  play(work_sound);
  loop(work_min);
  rest();
}

void rest() {
  clear();
  p("Starting rest session");
  play(rest_sound);
  loop(rest_min);
  work();
}

int main() {
  work();
}
