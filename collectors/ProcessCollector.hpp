#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
using namespace std;
using namespace std::filesystem;

class Process {
private:
  const int pid;
  string name = "";
  string command = "";
  double cpuUsage = 0.0;
  int memoryUsage = 0;

public:
  Process(int pid, const string &name, const string &command, double cpuUsage,
          int memoryUsage)
      : pid(pid), name(name), command(command), cpuUsage(cpuUsage),
        memoryUsage(memoryUsage) {}
  Process(int pid) : pid(pid){}

    void DisplayInfo() const {
    cout << "PID: " << this->pid << '\n';
    cout << "Name: " << this->name << '\n';
    cout << "Command: " << this->command << '\n';
    cout << "CPU Usage: " << this->cpuUsage << '\n';
    cout << "Memory Usage: " << this->memoryUsage << endl;
  }

  int getPid() const { return pid; }
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

  void update() {
    int counter = 0;
    for (auto &entry : directory_iterator(_procPath)) {
      if (isdigit(entry.path().stem().string()[0])) {
        const string pid = entry.path().stem().string();
        Process process = MakeProcess(pid);
        _processList.push_back(process);
        counter++;
      }
    }
  }
  Process MakeProcess(string pid) {
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
      throw runtime_error("File could not be opened");
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
      throw runtime_error("File could not be opened");
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
      throw runtime_error("File could not be opened");
    }
    getline(file3, content);
    string command = content;
    file3.close();
    return Process(stoi(pid), name, command, utime + stime, memoryUsage * 4);
  }

  void const DisplayProcesses(int limit = -1) const {
    if (limit < 0) {
      limit = _ProcessCount();
    }
    int count = 0;
    for (const auto &process : _processList) {
      if(process.getPid() == 0) {
        continue; // Skip the process with PID 0
      }
      process.DisplayInfo();
      count++;
      if (count >= limit) {
        break;
      }
    }
  }

private:
  path _procPath = "/proc";
  vector<Process> _processList = {};
  int _ProcessCount() const { return static_cast<int>(_processList.size()); }
};

