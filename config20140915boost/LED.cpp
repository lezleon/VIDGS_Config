#include "StdAfx.h"
#include "VIDGS_serial.h"
#include "LED.h"
#include "Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

boost::asio::io_service led_iosev;
boost::shared_ptr<boost::asio::serial_port > glb_led_ptr;
commu_interface glb_led_data;
boost::shared_ptr<LED> glb_led_obj_ptr;
string glb_plane_type;
string glb_plane_position;
string glb_flight_number;

LED::LED()
{
	PCode="00";
	PType="0000";
	PDistance="00.0";
	PTime="00:00:00";
	PStart = "(("; 
	//PFlag = "0001";
	PEnd = "))";
	send_Mark=0;
}


LED::~LED()
{
}

void LED::Com_Led_Write()
{
	boost::posix_time::ptime start_time,end_time;
	ptrMsgData curSend=NULL;
	int sent_N = 0,curSent=0;
	boost::system::error_code err;
	while(1)
	{
		boost::this_thread::interruption_point();
		{
		mutex::scoped_lock lock(glb_led_data.sendMu);
		glb_led_data.con_send.wait(glb_led_data.sendMu);
		}
		try{

		if (glb_led_data.pSend_data_.size()>0)
		{
			ptrMsgData pPack = glb_led_data.getFromSendList_checkCount();
		}
		for (int i=0;i<glb_led_data.pSend_data_.size();i++)
		//for(circular_buffer<ptrMsgData>::iterator curSend = glb_led_data.pSend_data_.begin();curSend!=glb_led_data.pSend_data_.end();curSend++)
		{
			 curSend = glb_led_data.pSend_data_[i];

			 //只发送一次的程序，间隔40ms
			 if (curSend->N_send==0 )
			 {
				 //sent_N = 0;
				 //while (sent_N<curSend->lenData)
				 //{
					// curSent = glb_led_ptr->write_some(buffer(curSend->msgData+sent_N,curSend->lenData-sent_N),err);
					// sent_N+=curSent;
					// boost::this_thread::interruption_point();
				 //}
				 memcpy(LastMessage,curSend->msgData,curSend->lenData);
				 LastMessageLen = curSend->lenData;
				 boost::asio::write(*glb_led_ptr,buffer(curSend->msgData,curSend->lenData));
				  //LOG(INFO)<<"send"<<curSend->msgData<<endl;
				  this_thread::sleep(boost::posix_time::millisec(LEDDelayTime));
				
				 curSend->N_send = 3;//只发一次的设置				 
			}
		}
		}
		catch(std::exception e)
		{

		}
	}

}

int LED::unpackHead(unsigned char * led_buf)
{
	//解析正确则返回下次要接收的长度
	unsigned char zero_n='0';
	unsigned char five_n='5';
	int nextdata=(led_buf[2]-zero_n)*10+led_buf[3]-zero_n;
	if (33 == nextdata)
	{
		led_buf[2]=led_buf[2]+five_n;
		return nextdata;
	}
	else if (nextdata>=50)
	{
		return 2;
	}
	else 
	{
		return 0;//解析错误
	}
}

int LED::unpackBody(unsigned char * led_buf,int code_length)
{
	//解析正确则返回下次要接收的长度
	if (')'==led_buf[code_length-2]&&')'==led_buf[code_length-1])
	{
		return code_length;
	}
	else
	{
		return 0;//解析错误
	}

}
#define LED_READDATA {readN = glb_led_ptr->read_some(buffer(com_led_buf+nTotalLen, commu_interface::max_onePack_length-nTotalLen), err);\
	nTotalLen += readN; leftByte+=readN;\
	boost::this_thread::interruption_point();\
	string temp;temp.append((const char*)com_led_buf+nTotalLen-readN,readN);}
//LOG(INFO)<<"LED read: "<<temp<<endl;}
//	std::string t = to_iso_string(boost::posix_time::microsec_clock::local_time());\
//TRACE("%s LED受到：",t.c_str());\
//for (int i=nTotalLen-readN;i<nTotalLen;i++)\
//	TRACE("%c",com_led_buf[i]);TRACE("\n");}LOG(INFO)<<"LED read: "<<curSend->msgData<<endl;
void LED::Com_Led_Read()
{
	unsigned char com_led_buf[commu_interface::max_onePack_length],com_led_temp_trans_buf[commu_interface::max_onePack_length];
	int nNextLen = 1;
	size_t readN=0;
	int nTotalLen=0;//当前内存存储的未处理字节总数
	//int data_length;
	int usedByte = 0;//统计已经用了多少字节
	int leftByte = 0;//还剩多少字节已经读入内存，但没有处理
	//double time_start=-10;
	//double time_end;
	//int count_pack=0;
	int tempUse;
	boost::system::error_code err;
	int Link_order=0;
	while (1)
	{		
		boost::this_thread::interruption_point();
		try {
			while (leftByte==0) LED_READDATA
			nNextLen = 1;
			//readN = boost::asio::read(*glb_led_ptr, buffer(com_led_buf+nTotalLen, nNextLen));
            
			//TRACE("%02x",com_led_buf[0]);
			//if (com_led_buf[0]==0x53) 
			//{//nTotalLen=0;
			//TRACE(",len:%d\n",nTotalLen);}
			//else
			//nTotalLen += readN;
			
			if ((1 <= leftByte) &&'('==com_led_buf[usedByte])
			{
				usedByte +=1;//已经判断了一个
				leftByte -= 1;

				Link_order=0;
				//nNextLen=1;
				//nTotalLen=1;

				while(1 > nTotalLen-usedByte) LED_READDATA
				//com_led_buf[0]='(';
				//readN = boost::asio::read(*glb_led_ptr, buffer(com_led_buf+nTotalLen, nNextLen));
				//TRACE("\nrecv head %c\n",com_led_buf[1]);
				if ((1 <= leftByte) &&'('==com_led_buf[1])
				{
					usedByte +=1;//已经判断了2个
					leftByte -= 1;

					nNextLen = 2;
					//nTotalLen+=readN;
					//readN = boost::asio::read(*glb_led_ptr, buffer(com_led_buf+nTotalLen, nNextLen));
					while (nNextLen > nTotalLen-usedByte)	LED_READDATA
					if (nNextLen  <= leftByte && (Link_order =unpackHead(com_led_buf)))//如果包头解析错误，则返回0
					{//unpackHead里面的+5有问题
						leftByte -= 2;
						usedByte += 2;//unpackHead要用nNextLen=2个
						//包头解析正确，
						//TRACE("\nrecv head unpack sucess %d\n",nNextLen);
						//nTotalLen+=readN;
						nNextLen = 1;
					}
				}						
			}
			else if (Link_order)
			{
				if (/*nTotalLen<26 && */nNextLen<=leftByte && ')'==com_led_buf[usedByte])//nTotalLen]) //解析数据，如果出错，则返回0
				{//不再判断<26
					usedByte +=1;//
					leftByte -= 1;
					nNextLen = 1;
					//TRACE("\nrecv body unpack sucess %d\n",nNextLen);
					//nTotalLen+=readN;
					//readN = boost::asio::read(*glb_led_ptr, buffer(com_led_buf+nTotalLen, nNextLen));
					while (nNextLen > nTotalLen-usedByte)	LED_READDATA
					active_time = boost::posix_time::microsec_clock::local_time();
						if ((nNextLen <= leftByte) && ')'==com_led_buf[usedByte])//nTotalLen])
						{
							usedByte +=1;//
							leftByte -= 1;
							active_time = boost::posix_time::microsec_clock::local_time();
							// receive ok.
							// 如果指令编码 大于50（字符到数值转换），就是反馈指令，则需要把对应的包count=3
							//查找反馈指令方法，在当前指令的“发送方报文标记”进行检索
							// 如果是33 ，则反馈，指令码改成83
							// 如果是小于33，则可能发送方出错
							//com_led_buf[nTotalLen+1]=0;
							//TRACE("\n%s",com_led_buf);
							//nTotalLen+=readN;
							if (Link_order == 33)
							{
								const unsigned char ContinueCmd[] = "((83))";
								//glb_led_data.deliver2send(ContinueCmd,6);
								boost::this_thread::sleep(boost::posix_time::milliseconds(LEDDelayTime));
								memcpy(LastMessage+12,to_iso_extended_string(boost::posix_time::microsec_clock::local_time()).substr(11,8).c_str(),8);
								glb_led_data.deliver2send(LastMessage,LastMessageLen,true,false);
								//LOG(INFO)<<"send 83."<<endl;
								/*sendmessage(83,0.0,"00:00:00","null");*/
								if (leftByte>0)
								{				
									memcpy(com_led_temp_trans_buf,com_led_buf+usedByte,leftByte);
									memcpy(com_led_buf,com_led_temp_trans_buf,leftByte);
								}
								nTotalLen=leftByte;
								usedByte=0;
								//if (leftByte==0)
								//{
								//	nTotalLen=0;
								//	//usedByte=0;//【】
								//	//leftByte=0;
								//}
							}
							else if (Link_order)
							{
								for (int i=0;i<glb_led_data.pSend_data_.size();i++)
									//for(circular_buffer<ptrMsgData>::iterator curSend = glb_led_data.pSend_data_.begin();curSend!=glb_led_data.pSend_data_.end();curSend++)
								{
									ptrMsgData curSend = glb_led_data.pSend_data_[i];
									if (usedByte==curSend->lenData && CompareFlag(com_led_buf,curSend->msgData,curSend->lenData))
									{//原来是nTotalLen==curSend->lenData
										curSend->N_send=3;
									}
								}
								if (leftByte>0)
								{
									memcpy(com_led_temp_trans_buf,com_led_buf+usedByte,leftByte);
									memcpy(com_led_buf,com_led_temp_trans_buf,leftByte);
									//for(int kk = 0; kk<leftByte; kk++)
									//{
									//	com_led_temp_trans_buf[kk] = com_led_buf[usedByte + kk];
									//}
									//for(int kk = 0; kk<leftByte; kk++)
									//{
									//	com_led_buf[kk] = com_led_temp_trans_buf[kk];
									//}
								}
								nTotalLen=leftByte;
								usedByte=0;
							}
							Link_order = 0;
							
						}					
				}
				else//没读到")"，读下一个字节
				{
					//nTotalLen+=readN;
					usedByte +=1;
					leftByte -= 1;
					nNextLen = 1;
				}				
			}//Link_order
			/*com_led_buf[nTotalLen]='\0';
			LOG(INFO)<<"recieve message:"<<com_led_buf<<endl;*/			
			else if (usedByte==0 && leftByte != 0)//第一个字节错
			{
				if (leftByte-1>0)
				{				
					memcpy(com_led_temp_trans_buf,com_led_buf+1,leftByte-1);
					memcpy(com_led_buf,com_led_temp_trans_buf,leftByte-1);
				}
				nTotalLen=leftByte-1;
				leftByte -=1;
				usedByte=0;
			}

			if (usedByte<0)//程序逻辑严重错误
			{
				//LOG(INFO)<<"LedRead:usedByte="<<usedByte<<endl;
				usedByte=0;
				nTotalLen=0;
				leftByte=0;
			}
			//包头解析
			//收数据 readN = boost::asio::read(*glb_sp_ptr,buffer(com_LED_buf+packHead,nNextLen));
			//校验数据正确性 如果数据不正确则丢弃，
			//如果数据正确，则放到数据接收链表上并通知处理线程 glb_sp_data.deliver2recv(send_test_data,10);
			//如果正确则放在接收数据中

		}
		catch (std::exception& e)
		{
			TRACE("\nException: %s\n", e.what());
			active_time = boost::posix_time::not_a_date_time;
		}
	}
}
string LED::LEDInit()
{

	try{
		system_parameters para=Doc.readsystemdatas(".\\test.xml");//删Doc.readsystemdatas(PARAMETER_FILENAME)
		string LEDCOM=para.LEDCOM;
		int LEDPAUD=para.LEDPAUD;
		LEDDelayTime = para.LEDDelayTime;
		glb_led_ptr = boost::make_shared<boost::asio::serial_port>(led_iosev, LEDCOM);
		glb_led_ptr->set_option(boost::asio::serial_port::baud_rate(LEDPAUD));
		glb_led_ptr->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
		glb_led_ptr->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
		glb_led_ptr->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
		glb_led_ptr->set_option(boost::asio::serial_port::character_size(8));
		if(glb_led_ptr->is_open()){
			glb_thread_group.create_thread( boost::bind(&LED::Com_Led_Write,this) );
			glb_thread_group.create_thread( boost::bind(&LED::Com_Led_Read,this) );
			active_time = boost::posix_time::not_a_date_time;
		}
		else
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			if(glb_led_ptr->is_open()){
				glb_thread_group.create_thread( boost::bind(&LED::Com_Led_Write,this) );
				glb_thread_group.create_thread( boost::bind(&LED::Com_Led_Read,this) );
				active_time = boost::posix_time::not_a_date_time;
			}
			else
			{
				glb_led_ptr = nullptr;
				glb_led_obj_ptr = nullptr;
				active_time = boost::posix_time::not_a_date_time;
				return "FAIL";
			}
		}
		//sendmessage(1, 3.14, "16:08:19", "A320");
	}
	catch (std::exception& e)
	{
		glb_led_ptr = nullptr;
		glb_led_obj_ptr = nullptr;
		active_time = boost::posix_time::not_a_date_time;
		return e.what();
	}
	led_iosev.run();
	return "SUCESS";
}

void LED::sendmessage(int led_code,float led_dis,string led_time,string led_type)
{
	//UpdateData(TRUE); //读取编辑框内容
	//if(glb_Em_Stop) led_code = 10;

	if(led_dis>99.9) led_dis=99.9;
	CString  m_strTXData;
	PDistance.Format("%04.1f",led_dis);
	//PTime = to_iso_string(boost::posix_time::microsec_clock::local_time()).c_str();
	PTime=(led_time.empty()||led_time.length()!=8) ? 
		to_iso_extended_string(boost::posix_time::microsec_clock::local_time()).substr(11,8).c_str()
		:led_time.c_str();
	PType=(led_type.empty()||led_type.length()!=4) ? glb_plane_type.c_str():led_type.substr(0,4).c_str();
	if(PType.GetLength() != 4) PType = "NULL";
		PCode.Format("%02d",led_code);
		PFlag.Format("%04x",++send_Mark);
		m_strTXData=PStart+PCode+PType+PDistance+PTime+PFlag+PEnd;
		/*LOG(INFO)<<"LED message: "<<m_strTXData.GetBuffer()<<endl;*/
		if (send_Mark>65535)
		{
			send_Mark=0;
		}
     

	 glb_led_data.deliver2send((unsigned char*)m_strTXData.GetBuffer(),m_strTXData.GetLength(),true,false);
	 /*string now_time=to_iso_string(boost::posix_time::microsec_clock::local_time());
	 TRACE("%sLED请求发送:",now_time.c_str());
	 TRACE("%s\n",m_strTXData);*/
	/* for(int i=0;i<m_strTXData.GetLength();i++)
	 {
		  TRACE("s% ",m_strTXData[i]);
	 }*/
	// TRACE("\n\n",now_time.c_str());
	 //TRACE("s% ",m_strT
	 /*write(*glb_sp_ptr, buffer(senddata, len));
	 read(*glb_sp_ptr, buffer(buf, data_length));
	 OnCommand(buf);
	 read(*glb_sp_ptr, buffer(buf, data_length));
	 OnCommand(buf);*/

}

int LED::CompareFlag(unsigned char * receive_buf,unsigned char * cur_buf,int order_length)
{
	int flag=0;
	for (int i=order_length-6;i<order_length-2;i++)
	{
		flag++;
		if (receive_buf[i]!=cur_buf[i])
		{
			flag=0;
			break;
		}		
	}
	return flag;
}

void LED::SaveMessage()
{
	memcpy(LastMessage,SavedMessage,255);
	SavedMessageLen = LastMessageLen;
}
void LED::SendSaveMessage()
{
	glb_led_data.deliver2send(SavedMessage,SavedMessageLen,true,false);
}