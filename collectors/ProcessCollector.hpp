#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include"../Process.hpp"
using namespace std;
using namespace std::filesystem;

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

  vector<Process> getProcesses(int limit = 0, string sortBy = "pid", bool ascending = false){
    sort(sortBy,ascending);
    if(limit <= 0){
      return _processList;
    }else {
      vector<Process> temp;
      for(int i = 0; i < limit; i++){
        temp.push_back(_processList[i]);
      }
      return temp;
    }
  }

private:
  path _procPath = "/proc";
  vector<Process> _processList = {};
  int _ProcessCount() const { return static_cast<int>(_processList.size()); }

  void const sort(string sortBy = "pid",
                  bool ascending = false) {
    if (sortBy == "cpu") {
      Process::sort(_processList, Process::SortBy::cpu, ascending);
    } else if (sortBy == "mem") {
      Process::sort(_processList, Process::SortBy::mem, ascending);
    } else if (sortBy == "name") {
      Process::sort(_processList, Process::SortBy::name, ascending);
    } else {
      Process::sort(_processList, Process::SortBy::pid, ascending);
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
    return Process(stoi(pid), name, command, utime + stime, totalCpuUsage,
                   memoryUsage * 4, totalMemoryUsage);
  }
};
