#ifndef _EXT_DEV_ID_H
#define _EXT_DEV_ID_H
/*
* ��������� ��� ������ � ���������� �������������� ���������� (������������ � ��������� ������ ������ 3 � ����)
*/
typedef struct
{
	unsigned int reserve;
	unsigned short int VID;
	unsigned short int PID;
	unsigned int id;
} ExtDevId;

#endif