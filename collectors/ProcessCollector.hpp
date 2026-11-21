#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
using namespace std;
using namespace std::filesystem;

class Process {
public:
  Process(int pid, const string &name, const string &command, int cpuUsage, int totalCpuUsage,
          int memoryUsage, int totalMemoryUsage)
      : pid(pid), name(name), command(command), cpuUsage(cpuUsage), totalCpuUsage(totalCpuUsage),
        memoryUsage(memoryUsage), totalMemoryUsage(totalMemoryUsage) {}
  Process(int pid) : pid(pid) {}

  void DisplayInfo() const {
    cout << "PID: " << this->pid << '\n';
    cout << "Name: " << this->name << '\n';
    // cout << "Command: " << this->command << '\n';
    cout << "CPU Usage: %" << (this->cpuUsage/(double)totalCpuUsage) * 100 << '\n';
    cout << "Memory Usage: %" << (this->memoryUsage/(double)totalMemoryUsage)*100 << endl;
    cout << "******************************************************" << '\n';
  }
  int getPid() const { return pid; }

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

class ProcessCollector {
public:
  ProcessCollector(path procPath = "/proc") {
    _procPath = procPath;
    if (!exists(procPath) || !is_directory(procPath)) {
      throw runtime_error(
          "/proc directory does not exist or is not a directory");
    }
  }

  void update(int totalCpuUsage, int totalMemoryUsage) {
    _processList.clear();
    int counter = 0;
    for (auto &entry : directory_iterator(_procPath)) {
      if (isdigit(entry.path().stem().string()[0])) {
        const string pid = entry.path().stem().string();
        Process process = MakeProcess(pid, totalCpuUsage, totalMemoryUsage);
        if (process.getPid() == 0)
          continue;
        _processList.push_back(process);
        counter++;
      }
    }
  }
  Process MakeProcess(string pid, int totalCpuUsage, int totalMemoryUsage) {

    path dir = "/proc/" + pid;
    string word;
    int count = 0;
    int utime = 0;
    int stime = 0;

    if (!is_directory(dir) || !exists(dir)) {
      return Process(0);
    }

    // cpu stats
    ifstream file(string(dir) + "/stat");
    if (!file.is_open()) {
      file.close();
      return Process(0);
    }
    string content((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    // Process Name
    size_t openParen = content.find('(');
    size_t closeParen = content.rfind(')');
    string name = content.substr(openParen + 1, closeParen - openParen - 1);

    stringstream iss(content.substr(closeParen + 4));
    while (iss >> word) {
      if (count == 10) {
        utime = stoi(word);
      } else if (count == 11) {
        stime = stoi(word);
      }
      count++;
    }
    file.close();

    ifstream file2(string(dir) + "/statm");
    if (!file2.is_open()) {
      file2.close();
      return Process(0);
    }
    getline(file2, content);
    stringstream iss2(content);
    int memoryUsage;
    count = 0;
    while (iss2 >> word) {
      if (count++ == 1) {
        memoryUsage = stoi(word);
        break;
      }
    }
    file2.close();

    ifstream file3(string(dir) + "/cmdline");
    if (!file3.is_open()) {
      file3.close();
      return Process(0);
    }
    getline(file3, content);
    string command = content;
    file3.close();
    return Process(stoi(pid), name, command, utime + stime, totalCpuUsage, memoryUsage * 4, totalMemoryUsage);
  }

  void const DisplayProcesses(int limit = 0, string sortBy = "pid",
                              bool ascending = false) {
    if (limit == 0) {
      limit = _ProcessCount();
    }
    if(sortBy == "cpu"){
      Process::sort(_processList, Process::SortBy::cpu, ascending);
    }else if(sortBy == "mem"){
      Process::sort(_processList, Process::SortBy::mem, ascending);
    }else if(sortBy == "name"){
      Process::sort(_processList, Process::SortBy::name, ascending);
    }else{
      Process::sort(_processList, Process::SortBy::pid, ascending);
    }

    int count = 0;
    for (const auto &process : _processList) {
      if(count > limit){
        break;
      }
      process.DisplayInfo();
      count++;
    }
  }

private:
  path _procPath = "/proc";
  vector<Process> _processList = {};
  int _ProcessCount() const { return static_cast<int>(_processList.size()); }
};
