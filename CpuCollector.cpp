#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sstream>

using namespace std;
class CpuCollector {
public:
  CpuCollector() {
    previousUsage = getUsage();
    previousIdle = getIdle();
  }

  void update() {
    currentUsage = getUsage();
    currentIdle = getIdle();
  }
  const double getUsage() {
    ifstream file("/proc/stat");
    if (!file.is_open()) {
        cout << "Error: Could not open /proc/stat" << endl;
      return -1;
    }
    
    string line;
    getline(file, line);
    istringstream iss(line);
    double sum =0;
    string word;
    while(iss >> word) {
        if(word != "cpu"){
            sum += stod(word);
        }
    }    

    file.close();
    return sum;
  }
  const double getIdle(){
    ifstream file("/proc/stat");
    if (!file.is_open()) {
        cout << "Error: Could not open /proc/stat" << endl;
      return -1;
    }
    string line;
    getline(file, line);
    double idle = 0;
    istringstream iss(line);
    string word;
    int count = 0;
    while(iss >> word) {
        if(count == 4){
            idle = stod(word);
            break;
        }
        count++;
    }

    file.close();
    return idle;
  }

  const double getUtilization(){
    currentUsage = getUsage();
    currentIdle = getIdle();
    double usageDiff = currentUsage - previousUsage;
    double idleDiff = currentIdle - previousIdle;
    double utilization = (1.0- (idleDiff/usageDiff)) * 100;
    previousUsage = currentUsage;
    previousIdle = currentIdle;
    return utilization;
  }

private:
  double previousUsage;
  double currentUsage;
  double previousIdle;
  double currentIdle;
};

int main() {
  CpuCollector cpuCollector;
  while (true) {
    cpuCollector.update();
    double usage = cpuCollector.getUsage();
    double idle = cpuCollector.getIdle();
    cout << "Usage: " << usage << endl;
    cout << "Idle: " << idle << endl;
    cout << "CPU Utilization: " << cpuCollector.getUtilization() << "%" << endl;
    sleep(2); // Sleep for 2 seconds before the next update
  }
}