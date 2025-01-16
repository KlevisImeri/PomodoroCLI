/*
 * MIT License
 *
 * Copyright (c) 2025 Klevis Imeri
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// clang-format off
#include <bits/stdc++.h>
#include <chrono>
#include <csignal>
#include <ctime>
#include <sqlite3.h>
#include <string>
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
const int work_min = 10;
const int rest_min = 5;
const str work_sound = "resources/'A dream…I saw a dream (subaru).mp3'";
const str rest_sound = "resources/'never meant to belong.mp3'";
const int FPS = 10;
const time_t oneday = 24*60*60;

// Colors
const str blue_light = "\033[94m";
const str red_light = "\033[91m";
const str green_light = "\033[92m";
const str yellow_light = "\033[93m";
const str magenta_light = "\033[95m";
const str cyan_light = "\033[96m";
const str color_reset = "\033[0m";

// UI Data
const str ui_full = "█";
const str ui_empty = "░";
str ui_title;
str ui_title_color = magenta_light;
int ui_total_min = 0;
str ui_total_color = magenta_light; 
int ui_progres;
vec<str> ui_progbar;

// DB
sqlite3* db;

str to_str(time_t time) { 
    tm* localTime = localtime(&time);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", localTime);
    return str(today);
}

void db_log(int exit_code, str good) {
  if (exit_code != SQLITE_OK) {
    d(sqlite3_errmsg(db));
  } else {
    d(good);
  }
}

int db_connect() {
  d("Connecting to db...");
  int exit = sqlite3_open("pomodoro.db", &db);
  db_log(exit,"DB connected!");
  return exit;
}

void db_close() {
  d("Closing connection to db...");
  sqlite3_close(db);
  d("Connection to db closed!");
}

void db_create() {
  d("Inicialzing db...");

  const char* sql = R"(
    CREATE TABLE IF NOT EXISTS day (
      ID INTEGER PRIMARY KEY,      
      date TEXT, 
      total_min INTEGER NOT NULL
    );
  )";

  int exit = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
  db_log(exit, "Db inicialized!");
}

int db_day_select(str day, int* total_min) {
    d("Selecting total_min for Day: " + day);

    const char* sql = R"(
        SELECT total_min
        FROM day
        WHERE date = ?;
    )"; 
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, day.c_str(), -1, SQLITE_STATIC);
    
    *total_min = 0;  // Default if not found
    int exit = sqlite3_step(stmt);
    if (exit == SQLITE_ROW) {
        *total_min = sqlite3_column_int(stmt, 0);  // Retrieve total_min from the first column
    }

    db_log(exit, "DAY Total_min Retrieved: ");
    sqlite3_finalize(stmt);
    return exit;
}

int db_day_insert(str day, int total_min) {
  d("Inserting Day:" + day + ", " + to_string(total_min)); 
  const char* sql = R"(
    INSERT INTO day (date, total_min)
    VALUES (?, ?)
  )";
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, day.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, total_min);

  int exit = sqlite3_step(stmt);

  db_log(exit, "DAY Inserted"); 
  sqlite3_finalize(stmt);
  return exit;
}

int db_day_update(str day, int total_min) {
  d("Updating Day:" + day + ", " + to_string(total_min));
  const char* sql = R"(
    UPDATE day
    SET total_min = ?
    WHERE date = ?;
  )";
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, total_min);
  sqlite3_bind_text(stmt, 2, day.c_str(), -1, SQLITE_STATIC);
  int exit = sqlite3_step(stmt);
  db_log(exit, "DAY Updated"); 
  sqlite3_finalize(stmt);
  return exit;
}

void db_save() { 
  d("Saving into the databse ...");
  if(db_connect() != SQLITE_OK) return;
  
  //TODO: assume you can study for more than 2 days 
  time_t now = time(0);
  time_t adjusted = now - ui_total_min*60;
  tm now_tm = *localtime(&now);
  tm adjusted_tm = *localtime(&adjusted);
  if (now_tm.tm_yday != adjusted_tm.tm_yday) {
    auto prev_day_min = ui_total_min - (now_tm.tm_hour * 60 + now_tm.tm_min);
    auto today_min = ui_total_min - prev_day_min; 
    db_day_update(to_str(time(0)-oneday), prev_day_min);
    db_day_insert(to_str(time(0)), today_min);
  } else {
    db_day_update(to_str(time(0)), ui_total_min);
  }
  d("Saved to db!");
}

void db_load() {
  d("Loading data from db...");
  if(db_connect() != SQLITE_OK) return;

  db_create();

  str today = to_str(time(0));
  int exit = db_day_insert(today, ui_total_min);
  if(exit != SQLITE_OK) {
    db_day_select(today, &ui_total_min);
  }

  d("Data loaded from db!");
}


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
  int hours = ui_total_min/60;
  int min = ui_total_min%60;
  cout<< "Total: "<<magenta_light<<hours<<':'<<min<<color_reset<<endl;
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
    for(int i=0; i<FPS; i++) {
      UI();
      sleep_for(seconds(60/FPS));
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
  ui_total_min += work_min;
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
  db_load();
  signal(SIGINT, [](int signal){
          db_save();
          exit(0);
        });
  work();
}
