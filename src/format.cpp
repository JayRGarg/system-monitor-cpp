#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;
using std::ostringstream;
using std::setw;
using std::setfill;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    long hours = seconds / 3600;
    long minutes = (seconds % 3600) / 60;
    long secs = seconds % 60;

    ostringstream oss;
    oss << setw(2) << setfill('0') << hours   << ":"
        << setw(2) << setfill('0') << minutes << ":"
        << setw(2) << setfill('0') << secs;

    return oss.str();
}