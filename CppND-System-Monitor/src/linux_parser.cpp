#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include <iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// return os information of system
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// return kernal information of system
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// return memoryUtilization of system
float LinuxParser::MemoryUtilization() { 
  string line, key;
  int value;
  std::map<string, int> meminfo;
  std::ifstream stream(kProcDirectory+kMeminfoFilename);
  if (stream.is_open()){
    while (std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      meminfo[key] = value; 
    }
  }
  float totalMemoryUse = meminfo["MemTotal:"]-meminfo["MemFree:"];
  float non_buffer_memory = totalMemoryUse - (meminfo["Buffers:"]+meminfo["Cached:"]);
  float buffer_memory = meminfo["Buffers:"];
  float cached_memory = meminfo["Cached:"]+meminfo["SReclaimable:"]-meminfo["Shmem:"];
  return (non_buffer_memory+buffer_memory+cached_memory)/meminfo["MemTotal:"]; 
  }

// return upTime of system
long LinuxParser::UpTime() { 
  string line;
  long int v1, v2;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> v1 >> v2;
  }
  return v1;
   }

// return total running processes on system
int LinuxParser::RunningProcesses() { 
  string line,key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running")
        return value;
    }
  }
  return 0; 
}

// return total processes on system
int LinuxParser::TotalProcesses(){
  string line,key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes")
        return value;
    }
  }
  return 0; 
}


// return vector of process id
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// return stat vector associated with process[pid]
vector<string> LinuxParser::process_stat(int pid) { 
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line,data;

  vector<string> info = {"0"};
  if (stream.is_open()){
      while (std::getline(stream,line)){
        std::istringstream linestream(line);
        while (linestream >> data){
          info.push_back(data);
        }
    }
 }
  return info;
}

// return cpuUtilization for each process[pid]
float LinuxParser::CpuUtilization(int pid) {
    vector<string> info = process_stat(pid);
    long int totaltime = stol(info[14]) + stol(info[15]) + stol(info[16]) + stol(info[17]);
    float uptime = LinuxParser::UpTime(); 
    
    //seconds = uptime - (starttime / Hertz)
    float secondsactive = uptime - (stol(info[22]) / sysconf(_SC_CLK_TCK) );
    float cpu_usage = (totaltime / sysconf(_SC_CLK_TCK)) / secondsactive;
    return cpu_usage;
  }

// return command asspciated with process[pid]
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    return line;
  }
  return line; 
  }

// return ram usage associated with process[pid]
string LinuxParser::Ram(int pid) { 
  string line,key,value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:"){
        return to_string(stol(value)/1024);
      }
    }
  }
  return string();
   }

// return Uid associated with process[pid]
string LinuxParser::Uid(int pid) { 
  string line,key,value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:"){
        return value;
      }
    }
  }
  return value;
 }

// return user associated with process[pid]
string LinuxParser::User(int pid) { 
  int uid;
  try{
    uid = stoi( LinuxParser::Uid(pid) );
  }
  catch(...){
    uid = -1;
  }

  string line,user="...",ignr;
  int upid;
  std::ifstream stream(kPasswordPath);

    if (stream.is_open()){
    while (std::getline(stream,line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> ignr >> upid;
      if (upid == uid){
        return user;
      }
        
    }
  }
  return user;
}

// return upTime of associted with process[pid]
long LinuxParser::UpTime(int pid) { 
  string line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    vector<string> temp;
    string word="";
    for(auto x : line){
      if (x == ' '){
        if (word != "")
          temp.push_back(word);
        word = "";
      }
      else{
        word = word + x;
      }
    }
    return stol(temp[21])/sysconf(_SC_CLK_TCK);
  }
  return 0; 
  }

  /*

long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return 0;
 }


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

*/