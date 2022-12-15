#include "supervisor.hpp"

#include <zf_log.h>


Supervisor::Supervisor() : thread(&Supervisor::supervisor, this)
{
    ;
}

Supervisor::~Supervisor()
{
    stop();
}

void Supervisor::set_limit(int limit_)
{
    if (limit_ < 2)
    {
        throw std::runtime_error("Invalid supervisor limit value " + std::to_string(limit_));
    }
    limit = limit_;
}

void Supervisor::stop()
{
    work = false;
    thread.join();
}

void Supervisor::acquire_resource(const std::string resource)
{
    if (!work) {
        return;
    }

    std::lock_guard<tthread::mutex> map_lock(map_mutex);

    if (resources.find(resource) == resources.end())
    {
        resources.insert(std::pair<std::string, int>(resource, 0));
    } 
    else
    {
        resources[resource] += 1;
    }
}

void Supervisor::supervisor()
{
    while (work)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT_MS));
        for (auto iter : resources)
        {
            if (iter.second > LIMIT)
            {
                ZF_LOGE("Too many unhandled device calls");
                ZF_LOGE("Slot %s will be reloaded by the supervisor", iter.first.c_str());
                
                int slot = std::stoi(iter.first);
                std::string slot_s = std::to_string(slot);
                system(("echo 0 > /opt/gpio/slot" + slot_s + "/value").c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(REBOOT_TIMEOUT_MS));
                system(("echo 1 > /opt/gpio/slot" + slot_s + "/value").c_str());
                {
                    std::lock_guard<tthread::mutex> map_lock(map_mutex);
                    if (resources.count(iter.first))
                    {
                        resources[iter.first] = 0;  // reset counter
                    }
                }
            }
        }
    }
}

void Supervisor::release_resource(const std::string resource)
{
    if (!work) {
        return;
    }

    std::lock_guard<tthread::mutex> map_lock(map_mutex);

    if (resources.find(resource) != resources.end())
    {
        resources[resource] -= 1;
        if (resources[resource] < 0)
        {
            resources.erase(resource);
        }
    }
    else
    {
        throw std::runtime_error("Resource " + resource + " not acquired");
    }
}

SupervisorLock::SupervisorLock(Supervisor *supervisor_, const std::string resource_) : supervisor(supervisor_), resource(resource_)
{
    supervisor->acquire_resource(resource);
}

SupervisorLock::~SupervisorLock()
{
    supervisor->release_resource(resource);
}
