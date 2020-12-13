#include "stdafx.h"
#include "TCPIPclient.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>//包含VECTOR类的库，必须包含近来

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void VIDGS_client::message_process(string message)
{
	//LOG(INFO)<<message<<endl;
	if(message[0]=='A'&&message.length()>34)
	{
		int commandId;
		char position[6],flight_number[7],plane_type[5],arrive_time[20];
		memcpy(position,message.c_str()+1,5);
		position[5]='\0';
		memcpy(flight_number,message.c_str()+8,6);
		flight_number[6]=0;
		memcpy(plane_type,message.c_str()+14,4);
		plane_type[4]=0;
		memcpy(arrive_time,message.c_str()+18,19);
		arrive_time[19]=0;
		commandId=atoi(message.substr(6,2).c_str());
		cout<<"message recieved:"<<endl;
		cout<<"position:"<<position<<endl;
		cout<<"command id:"<<commandId<<endl;
		cout<<"flight number:"<<flight_number<<endl;
		cout<<"plane type:"<<plane_type<<endl;
		cout<<"arrive time:"<<arrive_time<<endl;
		if(commandId<=MessageError) send_message(ClientStatus::CommandConfirmed,0);
		if(commandId<MessageError) send_message(ClientStatus::CommandConfirmed,0);
		if(commandId>ServerStatus::PositionAbandonCancel) 
		{
			send_message(ClientStatus::CommandConflict,0);
			return;
		}
		char tempc;
		switch(commandId)
		{
		case AskForPostionStatus:
			send_message(ClientStatus::PositionMaintain,0);
			break;
		case FlightComming:
			break;
		case FlightLeaving:
			break;
		case InformationChange:
			break;
		case FlightCanceled:
			break;
		case SetPositionAbandon:
			break;
		case MessageError:
			boost::this_thread::sleep(boost::posix_time::seconds(2));
			write_message((char *)last_message.c_str());
			break;
		case MessageComfirm:
			ReadSome();
			break;
		}
	}
	else
	{
		boost::this_thread::sleep(boost::posix_time::seconds(2));
		send_message(ClientStatus::CommandConflict,0); //异常 25
	}
}

void VIDGS_client::send_message(int status,int faultNum,string send_time)
{
	if(!connected) return;
	if((status>=1 && status <= 9)||(status>=13 && status <= 24)||(status>=29 && status <= 32))
	{
		current_status = (ClientStatus) status;
	}
	char message[38];
	if (send_time.empty())
	{
		string now_time=boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
		now_time[10]=' ';
		if(status==ClientStatus::SendFile)
			sprintf(message,"B%3s%02d%0*d%19s%02d",glb_plane_position.c_str(),status,10,faultNum,now_time.c_str(),0);
		else
			sprintf(message,"B%3s%02d%6s%4s%19s%02d",glb_plane_position.c_str(),status,glb_flight_number.c_str(),glb_plane_type.c_str(),now_time.c_str(),faultNum);
	}
	else
	{
		if(status==ClientStatus::SendFile)
			sprintf(message,"B%3s%02d%0*d%19s%02d",glb_plane_position.c_str(),status,10,faultNum,send_time.c_str(),0);
		else
			sprintf(message,"B%3s%02d%6s%4s%19s%02d",glb_plane_position.c_str(),status,glb_flight_number.c_str(),glb_plane_type.c_str(),send_time.c_str(),faultNum);
	}
	write_message(message);
	last_message=message;
}
void VIDGS_client::OnConnected()
{
	connected=true;
	//LOG(INFO)<<"server connected!"<<endl;
	string timestr=boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
	timestr[10]=' ';
	this_thread::sleep(boost::posix_time::seconds(2));
	send_message(ClientStatus::PositionMaintain,0,timestr);
	this_thread::sleep(boost::posix_time::seconds(1));
	//send_file("F:\\机场项目\\VIDGS_InfoCenter1228\\VIDGS_InfoCenter\\log\\INFO2014-05-07T11-35-24.txt");
	//send_file("1234.txt");
}
const unsigned int MAXPACK = 4096;
void VIDGS_client::send_file( string filePath )
{
	this_thread::sleep(boost::posix_time::seconds(1));
	ifstream in(filePath, ios::in);  
	istreambuf_iterator<char> beg(in), end;  
	string strdata(beg, end);//或者string st;st.assign(beg,end);  
	in.seekg(0);

	int pp=0,file_size = strdata.length();
	send_message(ClientStatus::SendFile,file_size);
	this_thread::sleep(boost::posix_time::seconds(1));
	do 
	{
		char buffer[MAXPACK+1];
		//in.read(buffer,min(MAXPACK,(unsigned int)(file_size-pp)));
		//socket_.write_some(boost::asio::buffer(buffer,min(MAXPACK,(unsigned int)(file_size-pp))));
		write_message((char *)(strdata.c_str()+pp),false,min(MAXPACK,(unsigned int)(file_size-pp)));
		/*if((file_size-pp)<MAXPACK)*/ TRACE("%s",strdata.c_str()+pp);
		pp+=min(MAXPACK,(unsigned int)(file_size-pp));
		this_thread::sleep(boost::posix_time::microseconds(10));
	} while (pp<file_size);
	this_thread::sleep(boost::posix_time::seconds(2));
	in.close();
}
VIDGS_client* glb_ptr_client;