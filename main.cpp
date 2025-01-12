// clang-format off
#include <bits/stdc++.h>
#include <chrono>
#include <termios.h>
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

// Settings
const int work_min = 30;
const int rest_min = 5;
const str work_sound = "resources/'A dream…I saw a dream (subaru).mp3'";
const str rest_sound = "resources/'never meant to belong.mp3'";
const int FPS = 10;

// Colors
const str blue_light = "\033[94m";
const str red_light = "\033[91m";
const str green_light = "\033[92m";
const str yellow_light = "\033[93m";
const str magenta_light = "\033[95m";
const str cyan_light = "\033[96m";
const str color_reset = "\033[0m";

// UI Data
const str ui_empty = "░";
const str ui_full = "█";
str ui_title;
str ui_title_color = magenta_light;
int ui_progres;
vec<str> ui_progbar;


// You need to install mpv
// TODO: Make global thread so only one sound is run 
void play(str res) { 
  thread([res] { //thread save copy
    string command = "mpv --no-terminal " + res;
    d(command);
    system(command.c_str());
  }).detach();
}

void clear() {
  cout << "\033[H\033[J";
}

void UI() {
  clear();
  cout<<ui_title_color<<ui_title<<color_reset<<endl;
  for(auto& s:ui_progbar) cout<<s;
  cout<<ui_progbar.size()-ui_progres<<endl;
}

vec<str> new_progbar(int size) {
  vec<str> progbar(size);
  for(auto& s:progbar) s = ui_empty;
  return progbar;
}


void loop(int goal) {
  ui_progres = 0; 
  ui_progbar = new_progbar(goal);
  UI();
  while(ui_progres != goal) {
    for(int i=0; i<FPS; i--) {
      UI();
      sleep_for(milliseconds(1000/FPS));
    }
    ui_progbar[ui_progres] = ui_full;
    ui_progres++;
    UI();
  }
}

void work() {
  clear();
  ui_title = "Work Session 🧠";
  ui_title_color = red_light;
  play(work_sound);
  loop(work_min);
  rest();
}

void rest() {
  clear();
  ui_title = "Rest Session 💪";
  ui_title_color = blue_light;
  play(rest_sound);
  loop(rest_min);
  work();
}

int main() {
  work();
}
