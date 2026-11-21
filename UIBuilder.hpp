#include <ncurses.h>
#include <string>
#include <vector>
#include "./Process.hpp"
using namespace std;
struct SystemData {
  double cpuUsage = 0;
  double memUsage = 0;
};

inline void draw_ui(SystemData sysd, vector<Process> processList) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols); // Get updated terminal dimensions
  WINDOW *header = newwin(3, cols, 0, 0);
  WINDOW *mainWin = newwin(4, cols, 3, 0);
  WINDOW *procWin = newwin(rows - 7, cols, 7, 0);

  // 2. Draw borders and content using each window's LOCAL coordinates
  box(header, 0, 0);
  mvwprintw(header, 1, 2, "System Monitor v1.0");

  box(mainWin, 0, 0);
  mvwprintw(mainWin, 1, 2, "CPU: %f", sysd.cpuUsage);
  mvwprintw(mainWin, 2, 2, "RAM: %f", sysd.memUsage);

  box(procWin, 0, 0);
  mvwprintw(mainWin, 2, 2, "RAM: %f", sysd.memUsage);

  // 1. Print the header with fixed column widths
  mvwprintw(procWin, 1, 2, "%-8s %-15s %-25s %6s %6s", "PID:", "Program:",
            "Command:", "CPU:", "MEM:"); // for(const auto &proc : processList){
  int row = 2;
  for(const auto proc : processList){
    mvwprintw(procWin, row,2, "%-8d %-15.15s %-25.25s %5.1f%% %5.1f%%",
              proc.getPid(),
              proc.getName().c_str(),
              proc.getCommand().c_str(),
              proc.getCpuUsage(),
              proc.getMemUsage());
    row++;
  }
  // Push updated layout to screen
  wnoutrefresh(header);
  wnoutrefresh(mainWin);
  wnoutrefresh(procWin);
  doupdate();
  refresh();
}
