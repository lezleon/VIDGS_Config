#pragma once

#include "stdafx.h"


/*
VIDGS all msg base class definition
*/
class VIDGS_msg 
	{
	public:
		//VIDGS_msg():lenData(0) {msgData=NULL;}
		explicit VIDGS_msg(int N, int inStartSign=0,int inPrior=0):
		lenData(N),prior(inPrior),startSign(inStartSign) //default construction
		{
			msgData = new unsigned char[N+1];
			memset(msgData,0,N+1);
			timeStamp = boost::posix_time::microsec_clock::local_time();
			N_send=0;
		}
		VIDGS_msg(VIDGS_msg & otherMsg)	 //construction from another msg
		{
			lenData -= otherMsg.lenData;prior=otherMsg.prior;startSign=otherMsg.startSign;
			msgData = new unsigned char[otherMsg.lenData];
			memcpy(msgData,otherMsg.msgData,otherMsg.lenData);
			timeStamp = boost::posix_time::microsec_clock::local_time();
			N_send = otherMsg.N_send;
		}
		~VIDGS_msg() {
			//if (msgData) 
			delete [] msgData;
		}
		unsigned char * msgData;// the stored msg data
		int lenData;   // length of msg data
		int prior;    // the priority of the msg
		int startSign;//not used, the start of the package, =1 for the first block of package ;=0 for the other block of packges
		boost::posix_time::ptime timeStamp;
		int N_send;  //send count. 20131211
	};
typedef boost::shared_ptr<VIDGS_msg> ptrMsgData;

class commu_interface
{
public:	
	enum { max_onePack_length = (boost::asio::detail::default_max_transfer_size)>>3 };
	circular_buffer<ptrMsgData> pSend_data_;
	circular_buffer<ptrMsgData> pRecv_data_;
	mutex sendMu,recvMu;
	condition_variable_any con_send,con_recv;
	commu_interface()
	{
		pRecv_data_.set_capacity(512);//max_cirBuff_length);
		pSend_data_.set_capacity(512);//max_cirBuff_length);
	}
	virtual ~commu_interface() {
		//µÈ´ý¶ÁÐ´Íê£¿
		//int i=0;
		//while (i<3)
		//{
		//	if (pRecv_data_.empty() && pSend_data_.empty())
		//		break;
		//	this_thread::sleep(boost::posix_time::seconds(1));
		//	++i;
		//}
		//if (i==3)
		//{
		//	if (!pRecv_data_.empty())
		//	TRACE("\nrecv not finish!\n");
		//	if (!pSend_data_.empty())
		//	TRACE("\nsend not finish!\n");
		//}
	};
	//virtual void deliver(ptrMsgData &inData) = 0;

	void Sum_Check(int len,unsigned char senddata[])
	{
		int i;
		char checksum;
		checksum=0;
		for (i=0;i<len-1;i++)
		{
			checksum += senddata[i];
		}
		checksum = checksum^0xff;
		checksum++;
		senddata[len-1]=checksum;
	}

	void deliver2send(unsigned char* Data,const int len_data,bool bInsertBack=true,bool sum_check=true) 
	{
		if(sum_check) Sum_Check(len_data,Data);
		int lenN = len_data/max_onePack_length,lastN = len_data%max_onePack_length;
		mutex::scoped_lock lock(sendMu);
		if (bInsertBack)  {
			for (int ni = 0;ni<lenN;++ni) {
				ptrMsgData pdata(new VIDGS_msg(max_onePack_length));
				memcpy((void *)pdata->msgData,(void *)Data,max_onePack_length);					
				pSend_data_.push_back(pdata);				
				Data = Data+max_onePack_length;
			}

			if (lastN >0)  {
				ptrMsgData pdata(new VIDGS_msg(lastN));
				memcpy((void *)pdata->msgData,(void *)Data,lastN);
				pSend_data_.push_back(pdata);
			}
		}
		else  {
			Data = Data+lenN*max_onePack_length;
			if (lastN >0) {
				ptrMsgData pdata(new VIDGS_msg(lastN));
				memcpy((void *)pdata->msgData,(void *)Data,lastN);
				pSend_data_.push_front(pdata);				
			}
			Data = Data-max_onePack_length;
			for (int ni = lenN-1;ni>=0;--ni) {
				ptrMsgData pdata(new VIDGS_msg(max_onePack_length));
				memcpy((void *)pdata->msgData,(void *)Data,max_onePack_length);
				pSend_data_.push_front(pdata);
				Data = Data-max_onePack_length;
			}			
		}
		con_send.notify_all();
	}
	void deliver2Recv(const unsigned char* Data,const int len_data,bool bInsertBack=true) 
	{
		int lenN = len_data/max_onePack_length,lastN = len_data%max_onePack_length;
		mutex::scoped_lock lock(recvMu);
		if (bInsertBack)  {
			for (int ni = 0;ni<lenN;++ni) {
				ptrMsgData pdata(new VIDGS_msg(max_onePack_length));
				memcpy((void *)pdata->msgData,(void *)Data,max_onePack_length);
				pRecv_data_.push_back(pdata);
				Data = Data+max_onePack_length;
			}

			if (lastN >0)  {
				ptrMsgData pdata(new VIDGS_msg(lastN));
				memcpy((void *)pdata->msgData,(void *)Data,lastN);
				pRecv_data_.push_back(pdata);
			}
		}
		else  {
			Data = Data+lenN*max_onePack_length;
			if (lastN >0) {
				ptrMsgData pdata(new VIDGS_msg(lastN));
				memcpy((void *)pdata->msgData,(void *)Data,lastN);
				pRecv_data_.push_front(pdata);
			}
			Data = Data-max_onePack_length;
			for (int ni = lenN-1;ni>=0;--ni) {
				ptrMsgData pdata(new VIDGS_msg(max_onePack_length));
				memcpy((void *)pdata->msgData,(void *)Data,max_onePack_length);
				pRecv_data_.push_front(pdata);
				Data = Data-max_onePack_length;
			}			
		}
		con_recv.notify_all();
	}
	ptrMsgData getFromRecvList(bool bDir=true,bool bDel=true) 
	{
		ptrMsgData reP=NULL;
		{
			mutex::scoped_lock lock(recvMu);
			if (pRecv_data_.empty()) return reP;
			if (bDir)
			{ reP= pRecv_data_.front(); if (bDel) pRecv_data_.pop_front();}
			else
			{ reP= pRecv_data_.back(); if (bDel) pRecv_data_.pop_back();}
		}
		return reP;
	}
	ptrMsgData getFromSendList(bool bDir=true,bool bDel=true) 
	{
		ptrMsgData reP=NULL;
		{
			mutex::scoped_lock lock(sendMu);
			if (pSend_data_.empty()) return reP;
			if (bDir)
			{ reP= pSend_data_.front(); if (bDel) pSend_data_.pop_front();}
			else
			{ reP= pSend_data_.back(); if (bDel) pSend_data_.pop_back();}
		}
		return reP;
	}
	ptrMsgData getFromSendList_checkCount(bool bDir=true) 
	{
		ptrMsgData reP=NULL;
		{
			mutex::scoped_lock lock(sendMu);
			if (pSend_data_.empty()) return reP;
			if (bDir)
			{ reP= pSend_data_.front(); if (reP->N_send>=2) pSend_data_.pop_front();}
			else
			{ reP= pSend_data_.back(); if (reP->N_send>=2) pSend_data_.pop_back();}
		}
		return reP;
	}
	//iterator getSendIterator()
	//	{
	//		return glb_led_data.pSend_data_->iterator();
	//}
	//private:

};