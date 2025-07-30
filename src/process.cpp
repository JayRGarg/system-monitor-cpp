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

Process::Process(int pid)
  : pid_(pid),
    command_(LinuxParser::Command(pid)), 
    ram_(std::stol(LinuxParser::Ram(pid))),
    user_name_(LinuxParser::User(pid)),
    up_time_(LinuxParser::UpTime(pid)) {
  long seconds = LinuxParser::UpTime() - up_time_;
  long total_time = LinuxParser::ActiveJiffies(pid);
  cpu_utilization_ = static_cast<float>(total_time) / static_cast<float>(seconds);
}

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_utilization_; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return std::to_string(ram_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_name_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return up_time_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.cpu_utilization_;
}