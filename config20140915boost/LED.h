#pragma once
#include "stdafx.h"
#include "Doc.h"
extern string glb_plane_type;
extern thread_group glb_thread_group;
extern boost::shared_ptr<boost::asio::serial_port > glb_led_ptr;

using namespace std;
class LED//:boost::enable_shared_from_this<LED>
{
public:
	LED();
	~LED();
public:
	void Com_Led_Write();//��LED��������
	void Com_Led_Read();//����LED����
	string LEDInit();//��������
	void sendmessage(int led_code,float led_dis,string led_time="",string led_type="");//��д����ָ��
	int unpackHead(unsigned char * led_buf/*,int order_length*/);//����LED������ִ�б���
	int unpackBody(unsigned char * led_buf,int code_length);//�ж��Ƿ���������
	int CompareFlag(unsigned char * receive_buf,unsigned char * cur_buf,int order_length);//�жϷ�����Ϣ�Ƿ�ƥ��
	void SaveMessage();
	void SendSaveMessage();
public:
	CString PStart;//���Ŀ�ʼ "(("
	CString PCode;//ִ�б���
	CString PType;//�ɻ��ͺ�
	CString PDistance;//�ɻ�����
	CString PTime;//У��ʱ��ֵ
	CString PFlag;//���ͷ����ı��
	CString PEnd;//���Ľ��� "))"
	unsigned long send_Mark;//���ͷ����
	unsigned char LastMessage[255];
	unsigned int LastMessageLen;
	unsigned char SavedMessage[255];
	unsigned int SavedMessageLen;
	double LEDDelayTime;
	Document Doc;
	boost::posix_time::ptime active_time;
};
extern boost::shared_ptr<LED> glb_led_obj_ptr;
