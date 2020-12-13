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
	void Com_Led_Write();//向LED发送数据
	void Com_Led_Read();//接收LED数据
	string LEDInit();//启动串口
	void sendmessage(int led_code,float led_dis,string led_time="",string led_type="");//编写发送指令
	int unpackHead(unsigned char * led_buf/*,int order_length*/);//处理LED反馈的执行编码
	int unpackBody(unsigned char * led_buf,int code_length);//判断是否完整接收
	int CompareFlag(unsigned char * receive_buf,unsigned char * cur_buf,int order_length);//判断反馈信息是否匹配
	void SaveMessage();
	void SendSaveMessage();
public:
	CString PStart;//报文开始 "(("
	CString PCode;//执行编码
	CString PType;//飞机型号
	CString PDistance;//飞机距离
	CString PTime;//校正时间值
	CString PFlag;//发送方报文标记
	CString PEnd;//报文结束 "))"
	unsigned long send_Mark;//发送方标记
	unsigned char LastMessage[255];
	unsigned int LastMessageLen;
	unsigned char SavedMessage[255];
	unsigned int SavedMessageLen;
	double LEDDelayTime;
	Document Doc;
	boost::posix_time::ptime active_time;
};
extern boost::shared_ptr<LED> glb_led_obj_ptr;
