#include <vector>
#include<string>
#include<algorithm>
#pragma once
using namespace std;

class Process {
public:
  int getPid() const { return pid; }
  double getCpuUsage() const {return ((double)cpuUsage/totalCpuUsage) * 100;}
  double getMemUsage() const {return ((double)memoryUsage/totalMemoryUsage) * 100;}
  string getName() const {return name;}
  string getCommand() const {return command;} 
  Process(int pid, const string &name, const string &command, int cpuUsage,
          int totalCpuUsage, int memoryUsage, int totalMemoryUsage)
      : pid(pid), name(name), command(command), cpuUsage(cpuUsage),
        totalCpuUsage(totalCpuUsage), memoryUsage(memoryUsage),
        totalMemoryUsage(totalMemoryUsage) {}
  Process(int pid) : pid(pid) {}

//   void DisplayInfo() const {
//     cout << "PID: " << this->pid << '\n';
//     cout << "Name: " << this->name << '\n';
//     // cout << "Command: " << this->command << '\n';
//     cout << "CPU Usage: %" << (this->cpuUsage / (double)totalCpuUsage) * 100
//          << '\n';
//     cout << "Memory Usage: %"
//          << (this->memoryUsage / (double)totalMemoryUsage) * 100 << endl;
//     cout << "******************************************************" << '\n';
//   }

  enum class SortBy {
    pid,
    cpu,
    name,
    mem,
  };

  static void sort(vector<Process> &processList, SortBy criterion = SortBy::pid,
                   bool ascending = true) {
    std::sort(processList.begin(), processList.end(),
              [criterion, ascending](const Process &a, const Process &b) {
                switch (criterion) {
                case SortBy::pid:
                  return ascending ? (a.pid < b.pid) : (a.pid > b.pid);
                case SortBy::cpu:
                  return ascending ? (a.cpuUsage < b.cpuUsage)
                                   : (a.cpuUsage > b.cpuUsage);
                case SortBy::mem:
                  return ascending ? (a.memoryUsage < b.memoryUsage)
                                   : (a.memoryUsage > b.memoryUsage);
                case SortBy::name:
                  return ascending ? (a.name < b.name) : (a.name > b.name);
                }
                return false;
              });
  }

private:
  int pid = 0;
  string name = "";
  string command = "";
  int cpuUsage = 0.0;
  int memoryUsage = 0;
  int totalCpuUsage = 0;
  int totalMemoryUsage = 0;
};
