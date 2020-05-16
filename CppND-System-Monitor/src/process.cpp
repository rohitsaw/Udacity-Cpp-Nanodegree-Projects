#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return this->pid; }

float Process::CpuUtilization() { return this->cpuUtil; }

string Process::Command() { return this->command; }

string Process::Ram() { return this->ram; }

string Process::User() { return this->user; }

long int Process::UpTime() { return this->uptime; }

bool Process::operator<(Process const& a) const { 
    if (this->cpuUtil > a.cpuUtil)
        return true;
    else
        return false;
    }

Process::Process(int p) {
    this->pid = p;
    this->command = LinuxParser::Command(p);
    this->user = LinuxParser::User(p);
    this->ram = LinuxParser::Ram(p);
    this->uptime = LinuxParser::UpTime(p);
    this->cpuUtil = LinuxParser::CpuUtilization(p);
}