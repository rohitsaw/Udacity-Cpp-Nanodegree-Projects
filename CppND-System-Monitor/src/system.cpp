#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

// includes
#include "linux_parser.h"
#include "format.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { 
    return cpu_; 
    }

vector<Process>& System::Processes() { 
    processes_.clear();
    vector<int> pids = LinuxParser::Pids();
    for(int pid : pids){
        processes_.emplace_back( Process(pid) );
    }
    sort(processes_.begin(), processes_.end());
    return processes_; 
    }

std::string System::Kernel() { 
    string kernal = LinuxParser::Kernel();
    return kernal;
 }

float System::MemoryUtilization() { 
    float memoryUtil = LinuxParser::MemoryUtilization();
    return memoryUtil;
 }

std::string System::OperatingSystem() { 
    string os = LinuxParser::OperatingSystem();
    return os; 
    }

int System::RunningProcesses() { 
    int runningProcesses = LinuxParser::RunningProcesses();
    return runningProcesses; 
    }

int System::TotalProcesses() { 
    int totalProcesses = LinuxParser::TotalProcesses();
    return totalProcesses;
 }

long int System::UpTime() { 
    long int uptime = LinuxParser::UpTime();
    return uptime; 
    }