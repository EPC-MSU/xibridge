#include <string>
#include <stdint.h>
#include <libserialport.h>

std::string serial_to_address(uint32_t serial);
uint32_t get_id_from_usb_location(const char *sp_port_name, bool& ok);
int initialization();