#include <fstream>
#include <sstream>

using namespace std;
class CpuCollector {
public:
  CpuCollector() {
    ReadCpuStats(previousTotal, previousIdle);
    currentTotal = previousTotal;
    currentIdle = previousIdle;
  }

  void update() {
    previousTotal = currentTotal;
    previousIdle = currentIdle;
    ReadCpuStats(currentTotal, currentIdle);
  }
  int getTotal() const { return currentTotal; }

  int getIdle() const { return currentIdle; }

  const double getUtilization() {
    currentTotal = getTotal();
    currentIdle = getIdle();
    double totalDiff = currentTotal - previousTotal;
    double idleDiff = currentIdle - previousIdle;
    if(totalDiff == 0) {
      return 0.0; // Avoid division by zero
    }
    double utilization = (1.0 - (idleDiff / totalDiff)) * 100;
    return utilization;
  }

private:
  double previousTotal = 0;
  double currentTotal = 0;
  double previousIdle = 0;
  double currentIdle = 0;

  void ReadCpuStats(double &total, double &idle) {
    ifstream file("/proc/stat");
    if (!file.is_open()) {
      printf("Error: Could not open /proc/stat");
      return;
    }

    string line;
    getline(file, line);
    istringstream iss(line);
    double sum = 0;
    string word;
    int count = 0;
    while (iss >> word) {
      if (word != "cpu") {
        sum += stod(word);
      }
      if (count == 4) {
        idle = stod(word);
      }
      count++;
    }
    total = sum;
    file.close();
  }
};