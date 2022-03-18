#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H

/* *
* ������� ������� ������ xibridge
*/
extern int xibridge_major_version();

/* *
* ������� ������� ������ ���������, �������������� ������ xibridge
*/
extern int xibridge_protocol_version();
/*
* ������� ������������� ������� xibridge 
*/
extern bool xibridge_init(const char *key_file_path);

/*
* ������� ���������� ������ � �������� xibridge
*/
extern void xibridge_shutdown();

/*
* ������� ��������� ������ ��������� ������� xibridge ��� ������� �����������
*
*/
extern void xibridge_set_server_protocol_verion(unsigned int conn_id, unsigned int ver);

/*
* ������� ������� ������ ��������� ������� ������� �����������
*/
extern unsigned int xibridge_get_server_protocol_version(unsigned int conn_id);

/*
  * ������� �������� ����������� � ������� (xi-net, xibridge)
  * To do timeout ? ? ?
*/
extern unsigned int xibridge_open_device_connection(const char *addr,
	unsigned int serial, unsigned int proto, unsigned int* last_errno = nullptr);

/*
 * ������� �������� ������� �����������
*/
extern void xibridge_close_device_connection(unsigned int conn_id);

/*
������� �������� ����������� � ������� (xi-net, xibridge)
*/
extern unsigned int xibridge_detect_protocol_version(const char *addr);

/*
* ������� ���������� �������
* To do timeout ? ? ?
*/
extern bool xibridge_device_request_response(unsigned int conn_id, 
	                                         const unsigned char *req, 
											 int req_len, unsigned char *resp, 
											 int resp_len 
											);

/*
* ������� ������������ ������ ������ �� �� ������
*/
extern void xibridge_get_err_expl(char * s, int len, bool is_russian, unsigned int err_no);

/*
������� ��������� ������ ������� ��������� ��������� 
*/
extern void xibridge_enumerate_devices(const char *addr,
    unsigned int proto,
    unsigned int *result,
    unsigned int *pcount,
    unsigned int* last_errno = nullptr);

/*
* ������� ��������� ������ ��������� ������ � ������� �����������
*/
extern unsigned int xibridge_get_last_err_no(unsigned int conn_id);
#endif