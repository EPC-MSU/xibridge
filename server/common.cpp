#include "common.hpp"

#include <zf_log.h>


size_t adaptive_wait_send(bindy::Bindy* bindy, conn_id_t conn_id, std::vector<uint8_t> data, size_t timeout_ms)
{
    bool send_ok = false;
    size_t delay = 2;
    size_t total_delay = 0;
    while(!send_ok && (total_delay + delay < timeout_ms))
    {
        try
        {
            bindy->send_data(conn_id, data);
            send_ok = true;
        }
        catch (const std::exception &e)
        {
            ZF_LOGE("Can\'t send data: %s", e.what());

            bindy::sleep_ms(delay);
            delay = static_cast<int>(delay * 3 / 2);
            total_delay += delay;
        }
    }
    if(!send_ok)
    {
        throw std::exception();
    }
    return total_delay;
}
