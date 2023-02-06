#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <ctime>
#include <algorithm>
#include <string>
#include <dirent.h>
#include <fstream>
#include <sstream>

// #include
// Function to get the current CPU usage of a process
double get_process_cpu_usage(pid_t pid)
{
    std::string stat_file = "/proc/" + std::to_string(pid) + "/stat";
    std::string stat_contents;
    std::ifstream stream(stat_file);
    std::getline(stream, stat_contents);
    stream.close();

    // Split the contents of the stat file by spaces
    std::vector<std::string> values;
    std::stringstream stat_stream(stat_contents);
    std::string buffer;
    while (stat_stream >> buffer)
    {
        values.push_back(buffer);
    }

    // Get the values of utime and stime
    long utime = std::stol(values[13]);
    long stime = std::stol(values[14]);
    long starttime = std::stol(values[21]);

    // Get the system uptime
    std::string uptime_file = "/proc/uptime";
    std::string uptime_contents;
    stream = std::ifstream(uptime_file);
    std::getline(stream, uptime_contents);
    stream.close();

    // Split the contents of the uptime file by spaces
    values.clear();
    std::stringstream uptime_stream(uptime_contents);
    while (uptime_stream >> buffer)
    {
        values.push_back(buffer);
    }

    // Get the value of uptime
    long uptime = std::stol(values[0]);

    // Calculate the total time the process has spent in user mode and kernel mode
    long total_time = utime + stime;

    // Calculate the seconds since the process started
    double seconds_since_start = uptime - (starttime / sysconf(_SC_CLK_TCK));

    // Calculate the current CPU usage of the process
    return ((total_time / sysconf(_SC_CLK_TCK)) / seconds_since_start) * 100;
}
double get_cpu_usage(pid_t pid) {
  std::string stat_file = "/proc/" + std::to_string(pid) + "/stat";
  std::ifstream stat_stream(stat_file);
  if (!stat_stream.is_open()) {
    std::cerr << "Failed to open " << stat_file << std::endl;
    return -1;
  }

  // Read the stat file and extract the relevant information
  int utime, stime, cutime, cstime;
  stat_stream >> utime >> stime >> cutime >> cstime;
  stat_stream.close();

  // Calculate the total CPU time used by the process
  int total_time = utime + stime + cutime + cstime;

  // Sleep for a short period of time
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Read the stat file again
  stat_stream.open(stat_file);
  if (!stat_stream.is_open()) {
    std::cerr << "Failed to open " << stat_file << std::endl;
    return -1;
  }

  // Read the stat file and extract the relevant information
  stat_stream >> utime >> stime >> cutime >> cstime;
  stat_stream.close();

  // Calculate the total CPU time used by the process after the sleep period
  int new_total_time = utime + stime + cutime + cstime;

  // Calculate the CPU usage as a percentage of total CPU time
  return (double)(new_total_time - total_time) / sysconf(_SC_CLK_TCK) / 0.1;
}
std::vector<pid_t> get_parent_pids(pid_t pid) {
  std::vector<pid_t> parent_pids;
  while (pid != 0) {
    pid_t parent_pid = getppid();
    parent_pids.push_back(parent_pid);
    pid = parent_pid;
  }
  return parent_pids;
}

// Function to get the child processes of a process
std::vector<pid_t> get_child_processes(pid_t pid)
{
    std::vector<pid_t> child_processes;

    // Get the path of the process's task directory
    std::string task_dir = "/proc/" + std::to_string(pid) + "/task/";

    // Get a list of all the subdirectories in the task directory
    DIR *dir = opendir(task_dir.c_str());
    struct dirent *entry = readdir(dir);
    while (entry != NULL)
    {
        // Check if the subdirectory is a valid process directory
        if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0]))
        {
            // Get the process ID of the child process
            pid_t child_pid = std::stoi(entry->d_name);

            // Get the parent process ID of the child process
            std::string status_file = "/proc/" + std::to_string(child_pid) + "/status";
            std::string status_contents;
            std::ifstream stream(status_file);
            std::string line;
            while (std::getline(stream, line))
            {
                if (line.find("PPid:") == 0)
                {
                    // Extract the parent process ID from the line
                    std::string ppid_str = line.substr(6);
                    pid_t ppid = std::stoi(ppid_str);

                    // Check if the parent process ID matches the given process ID
                    if (ppid == pid)
                    {
                        // Add the child process to the list of child processes
                        child_processes.push_back(child_pid);
                    }
                }
            }
            stream.close();
        }

        // Get the next subdirectory in the task directory
        entry = readdir(dir);
    }
    closedir(dir);

    return child_processes;
}

int main()
{
    // Get the current process ID
    pid_t pid = getpid();

    // Get the child processes of the current process
    std::vector<pid_t> child_processes = get_child_processes(pid);

    while (true)
    {
        // Check the CPU usage of each child process
        for (pid_t child_pid : child_processes)
        {
            double cpu_usage = get_process_cpu_usage(child_pid);
            if (cpu_usage > 90)
            {
                // High CPU usage detected, potentially malicious process
                std::cout << "High CPU usage detected: process " << child_pid << " is using " << cpu_usage << "% of the CPU." << std::endl;
            }
        }

        // Sleep for a short period of time before checking the CPU usage again
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
