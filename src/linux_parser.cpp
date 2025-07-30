#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  long value;
  std::unordered_map<std::string, long> memValues;
  long cached;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (!filestream.is_open()) {
        throw std::runtime_error("Failed to open file: " + kProcDirectory + kMeminfoFilename);
  }
  while (std::getline(filestream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> key >> value;
    memValues[key] = value;
  }
  cached = memValues["Cached"] + memValues["SReclaimable"] - memValues["Shmem"];
  return static_cast<float>(memValues["MemTotal"] - memValues["MemFree"] - memValues["Buffers"] - cached) / static_cast<float>(memValues["MemTotal"]);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime;
  string path{kProcDirectory + kUptimeFilename};
  std::ifstream filestream(path);
  if (!filestream.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
  }
  if (!std::getline(filestream, line)) {
    throw std::runtime_error("Failed to read line from file: " + path);
  }
  std::istringstream linestream(line);
  if (!(linestream >> uptime)) {
    throw std::runtime_error("Failed to parse uptime from line: '" + line + "' in file: " + path);
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  if (stream.is_open() && std::getline(stream, line)) {
    std::istringstream linestream(line);
    std::vector<std::string> values;
    std::string value;
    while (linestream >> value) {
      values.push_back(value);
    }
    if (values.size() >= 17) {
      long utime = std::stol(values[13]);
      long stime = std::stol(values[14]);
      long cutime = std::stol(values[15]);
      long cstime = std::stol(values[16]);
      return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
    }
  }
  return -1;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::vector<std::string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::string line;
    std::string value;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    while (linestream >> value) {
      jiffies.emplace_back(value);
    }
  }
  return jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (!filestream.is_open()) {
        throw std::runtime_error("Failed to open file: " + kProcDirectory + kStatFilename);
  }
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "processes") {
      return value;
    }
  }
  throw std::runtime_error("No processes info found in file: " + kProcDirectory + kStatFilename);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (!filestream.is_open()) {
        throw std::runtime_error("Failed to open file: " + kProcDirectory + kStatFilename);
  }
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "procs_running") {
      return value;
    }
  }
  throw std::runtime_error("No running processes info found in file: " + kProcDirectory + kStatFilename);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string memory = "";
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    std::string line;
    std::string key;
    std::string value;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // Using VmData instead of VmSize in order to get the exact physical
      // memory instead of the virtual memory
      if (key == kStatusVmData) {
        // Convert from KB to MB
        memory = std::to_string(std::stoi(value) / 1024);
        break;
      }
    }
  }
  return memory;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::string line;
    std::string up_time;
    std::getline(stream, line);
    std::istringstream linestream(line);
    const int up_time_pos = 22;
    for (int i = 0; i < up_time_pos; ++i) {
      linestream >> up_time;
    }
    return stol(up_time) / sysconf(_SC_CLK_TCK);
  }
  return -1;
}
