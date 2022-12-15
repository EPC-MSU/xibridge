#ifndef URPC_DEVXINET_SUPERVISOR_H
#define URPC_DEVXINET_SUPERVISOR_H

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <map>
#include <mutex>
#include <thread>

#include "bindy/tinythread.h"


class Supervisor
{
private:
    std::map<std::string, int> resources;
    tthread::mutex map_mutex;
    bool work = true;

    std::thread thread;

    void supervisor();

    const int TIMEOUT_MS = 3000;  // supervisor refresh timeout
    const int REBOOT_TIMEOUT_MS = 3000;  // slot reboot time
    const int LIMIT = 30;  // number of freezed connections allowed by supervisor

    int limit = LIMIT;

public:
    Supervisor();
    ~Supervisor();

    void acquire_resource(const std::string resource);
    void release_resource(const std::string resource);

    void stop();
    void set_limit(int limit_);
};

class SupervisorLock
{
private:
    Supervisor *supervisor;
    std::string resource;

public:
    SupervisorLock(Supervisor *supervisor_, const std::string resource);
    ~SupervisorLock();
};

#endif // URPC_DEVXINET_SUPERVISOR_H
