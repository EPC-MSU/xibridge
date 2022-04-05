#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H

#if defined (WIN32) || defined(WIN64)
//#if defined(xiwrapper_EXPORTS)
    #define XI_EXPORT __declspec(dllexport)
//#else
//#define BINDYC_EXPORT __declspec(dllimport)
//#endif
#else
   #define XI_EXPORT
#endif

#include "../common/ext_dev_id.h" 

#if defined(__cplusplus)
extern "C" {
#endif

#define TIMEOUT_3000 3000
/* *
* ������� ������� ������ xibridge
*/
int  XI_EXPORT xibridge_major_version();

/* *
* ������� ������� ������ ���������, �������������� ������ xibridge
*/
int  XI_EXPORT xibridge_protocol_version();
/*
* ������� ������������� ������� xibridge 
* ���� �������� 0 - �������
*/
int  XI_EXPORT xibridge_init(const char *key_file_path);


/*
* ������� ��������� ������ ��������� ������� xibridge ��� ������� �����������
*
*/
void  XI_EXPORT xibridge_set_server_protocol_verion(unsigned int conn_id, unsigned int ver);

/*
* ������� ������� ������ ��������� ������� ������� �����������
*/
unsigned int  XI_EXPORT xibridge_get_server_protocol_version(unsigned int conn_id);

/*
  * ������� �������� ����������� � ������� (xi-net, xibridge)
  *  ������� �������� �� ����� ������� � proto == 1 || proto == 2
  * To do timeout ? ? ? just receive timeout made
*/
unsigned int  XI_EXPORT xibridge_open_device_connection(const char *addr,
	unsigned int serial, unsigned int proto, unsigned int recv_timeout, unsigned int* last_errno);

/*
 * ������ ������ � �������� ����������� xibridge 
 */
int  XI_EXPORT xibridge_write_connection(unsigned int conn_id, const unsigned char *buf, int size);

/*
 * ������ ������ ������ �����������, � �������� - size bytes
 * ���������� �������� ���������� ������������
*/
int  XI_EXPORT xibridge_read_connection_buffer(unsigned int conn_id, unsigned char *buf, int size);

/*
* ������� �������� ����������� � �������  xibridge � ��������� ������������ �������������� ����������
* To do timeout ? ? ? just receive timeout made
* ������� �������� c �������� xibridge � proto >=3
* To do !!!
*/
//unsigned int  XI_EXPORT xibridge_open_device_connection_ext(const char *addr, const ExtDevId *pe_id,
//	                                                unsigned int proto, unsigned int recv_timeout, 
//													unsigned int* last_errno);
/*

/*
 * ������� �������� ������� �����������
*/
void  XI_EXPORT xibridge_close_device_connection(unsigned int conn_id);

/*
������� �������� ����������� � ������� (xi-net, xibridge)
*/
unsigned int  XI_EXPORT xibridge_detect_protocol_version(const char *addr, unsigned int send_timeout, unsigned int recv_timeout);

/*
* ������� ���������� �������
* To do timeout ? ? ? recieve timeout is as at 
* ��������� 0 - �������
*/
int  XI_EXPORT xibridge_device_request_response(unsigned int conn_id,
	                                         const unsigned char *req, 
											 int req_len, unsigned char *resp, 
                                             int resp_len, unsigned int *res_err
											);

/*
* ������� ������������ ������ ������ �� �� ������
*/
void  XI_EXPORT xibridge_get_err_expl(char * s, int len, int is_russian, unsigned int err_no);

/*
* ������� ��������� ������ ������� ��������� ��������� 
* ������� �������� c �������� ximc � �������� xibridge
*/
int  XI_EXPORT xibridge_enumerate_adapter_devices(const char *addr, const char *adapter,
	unsigned char *result,
	unsigned int *pcount, unsigned int timeout,
	unsigned int* last_errno);

/*
* ������� ��������� ������ ���������� ���������������  ��������� ���������
* ������� �������� � �������� xibridge
*/
void  XI_EXPORT xibridge_enumerate_devices_ext(const char *addr,
	unsigned int proto,
	ExtDevId *result,
	unsigned int *pcount,
	unsigned int* last_errno);

/*
* ������� ��������� ������ ��������� ������ � ������� �����������
*/
unsigned int  XI_EXPORT xibridge_get_last_err_no(unsigned int conn_id);

#if defined(__cplusplus)
};
#endif

#endif