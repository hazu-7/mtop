#include <fstream>
#include <sstream>
#include <unistd.h>
using namespace std;
class MemCollector {

public:
  MemCollector() { ReadMemStats(totalMemory, freeMemory); }

  void update() { ReadMemStats(totalMemory, freeMemory); }

  int getTotal() const { return totalMemory; }

  int getFree() const { return freeMemory; }

  void ReadMemStats(double &total, double &free) {
    ifstream file("/proc/meminfo");
    if (!file.is_open()) {
      printf("Error: Could not open /proc/meminfo");
      return;
    }

    string line;
    while (getline(file, line)) {
      istringstream iss(line);
      string key;
      double value;
      string unit;
      iss >> key >> value >> unit;

      if (key == "MemTotal:") {
        total = value; // in KB
      } else if (key == "MemAvailable:") {
        free = value; // in KB
      }
    }
  }

  double getUtilization() const {
    double usedMemory = totalMemory - freeMemory;
    if (totalMemory == 0) {
      return 0.0; // Avoid division by zero
    }
    double utilization = (usedMemory / totalMemory) * 100;
    return utilization;
  }

private:
  double totalMemory = 0;
  double freeMemory = 0;
};
