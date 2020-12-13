#pragma once
#include "stdafx.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Doc.h"

using boost::asio::ip::tcp;
using namespace std;
extern string glb_plane_type;
extern string glb_plane_position;
extern string glb_flight_number;

const enum ClientStatus
{
    PlanePositionEmpty=1, // 01：机位空，机位无飞机停靠且无预定航班	status
    PlaneTypeSetted, // 02：已预定航班，机位已预订航班但VIDGS没有开启 status
    VIDGSPrepared, // 03：进入捕获模式，VIDGS已开始工作但没有捕获到飞机	status
    Captured, // 04：捕获到飞机，向上位机发送捕获时间，此时VIDGS已进入引导识别阶段	status
    ValidationOK, // 05：机型验证成功，向上位机发送验证成功的时间，此时VIDGS进入精确引导阶段	status
    FrontWheelAligned, // 06：飞机第一次对准中心线，向上位机发送对准时的时间	status
    AccurateGuide, // 07：飞机距离停止线12m，发送该时刻的时间	status
    GuideSuccess, // 08：精确引导成功，飞机成功到达停止线，向上位机发送飞机停止时时间	status
    UrgencyStop, // 09：紧急停止（紧急停止按钮被人工按下）	status
    ValidationFail, // 10：机型验证失败
    AccurateGuideFail, // 11：精确引导失败
    PlaneTooFast, // 12：飞机速度过快
    PlaneStopped, // 13：飞机已泊位，机位被占用	status
    PlaneExiting, // 14：飞机离坞中 status
    PositionAbandon, // 15：机位禁用中 status
    PositionMaintain, // 16：机位维护中 status
    VIDGSMalfunction, // 17：VIDGS故障 status
    ForeignMatter, // 18：机位被相邻机位的飞机阻塞或者被障碍物阻塞 status
    CommunicationError, // 19：通信故障，上位机与VIDGS通信有故障 status
    Maintained, // 20：准备开启，VIDGS已安装或维修完，且上位机与VIDGS已联系上，但停机位未确定何时使用 status
    ManualStart, // 21：人工启动，VIDGS已由员工通过操作面板启动 status
    HasNotReachStopLine, // 22：泊位过程结束，但飞机未停止在停止线 status
    HasTrailer, // 23：无拖车 status
    NoTrailer, // 24：有拖车 status
	SysStop,  //25: 停止任务
    CommandConfirmed, // 26：回答服务器端发送来的消息，该消息发送正确，可以执行	
    CommandConflict, // 27：回答服务器端发送来的消息，该消息与机位现阶段状态冲突，不能执行
    SendFile, // 28: 表示VIDGS要向上位机发送日志文件，文件为TXT格式，以数据流的形式向上位机发送。VIDGS向上位机发送了此消息后，等待1秒，即可向上位机发送文件数据流
	ChockOn, // 29:轮挡已放置 status
	ChockOff, // 30:轮挡已移除 status
	ParkOn, // 31:手动确认飞机已停好 status
	ParkOff // 32:手动确认飞机已离开 status
};

const enum ServerStatus
{
    AskForPostionStatus=1, // 01：请求机位报告机位状态
    FlightComming, // 02：有航班将要泊位
    FlightLeaving, // 03：现有航班将要离开
    InformationChange, // 04：航班信息修改（例如到达时间修改）
    FlightCanceled, // 05：取消航班
    SetPositionAbandon, // 06：机位禁用
    MessageError, // 07：VIDGS发送信息错误，要求重发
	MessageComfirm, // 08：信息有效
	PositionAbandonCancel // 09:机位解禁
};

//const string PlaneType="A320";
//const string PlanePosition="B02";
//const string FlightNumber="SC0103";
#define USE_UNICODE 1
//#define max_length 1024

class client
{
public:
  client(boost::asio::io_service& io_service,
      const std::string& server, const std::string& port)
    : 
      resolver_(io_service),
      server_(server),
      port_(port)
  {
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
      error_time=0;
    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    tcp::resolver::query query(server, port);
    resolver_.async_resolve(query,
        boost::bind(&client::handle_resolve, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::iterator));
    //TRACE("get here");
	socket_.reset(new boost::asio::ip::tcp::socket(io_service));
	io_service_ = &io_service;
  }

  void handle_resolve(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator)
  {
      //TRACE("get here\n");
    if (!err)
    {
      // Attempt a connection to the first endpoint in the list. Each endpoint
      // will be tried until we successfully establish a connection.
      std::cout<<"Connecting to server..."<<endl;
      tcp::endpoint endpoint = *endpoint_iterator;
      socket_->async_connect(endpoint,
          boost::bind(&client::handle_connect, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
        error_time++;
        std::cout << "WARNING:Connect Server Error: " << err.message();
    }
  }

  bool is_open()
  {
	  return socket_->is_open();
  }

  void handle_connect(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator)
  {
      //TRACE("get here2\n");
    if (!err)
    {
        error_time=0;
        OnConnected();
        //cout<<"server connected"<<endl;
        //write_message("VIDGS connecting");
        /*socket_.async_read_some(boost::asio::buffer(response_w, max_length),
            boost::bind(&client::handle_read_welcome, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));*/
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
      // The connection failed. Try the next endpoint in the list.
      socket_->close();
      tcp::endpoint endpoint = *endpoint_iterator;
      socket_->async_connect(endpoint,
          boost::bind(&client::handle_connect, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
        error_time++;
        std::cout << "WARNING:Connect Server Error: " << err.message();
        std::cout << "Wait for reconnect "<<error_time<<"...";
        socket_->close();
        boost::this_thread::sleep(boost::posix_time::seconds(5));
        //std::cout << "Reconnecting "<<error_time<<"...\n";
        tcp::resolver::query query(server_, port_);
        resolver_.async_resolve(query,
            boost::bind(&client::handle_resolve, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::iterator));
      
    }
  }
  void handle_file_write_request(const boost::system::error_code& err)
  {
	   if (err)
	   {
		   std::cout << "WARNING:Error: " << err.message();
	   }
  }
  void handle_write_request(const boost::system::error_code& err)
  {
    if (!err)
    {
        error_time=0;
        socket_->async_read_some(boost::asio::buffer(
#ifdef USE_UNICODE
            response_w
#else
            response
#endif
            , max_length),
            boost::bind(&client::handle_read_response, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        
    }
    else
    {
        error_time++;
      std::cout << "WARNING:Error: " << err.message();
      if(error_time<10)
      {
#ifdef USE_UNICODE
          boost::asio::async_write(*socket_,boost::asio::buffer(request_w,request_w_length*2),
              boost::bind(&client::handle_write_request, this,
              boost::asio::placeholders::error));
#else
          boost::asio::async_write(*socket_,boost::asio::buffer(request,strlen(request)),
              boost::bind(&client::handle_write_request, this,
              boost::asio::placeholders::error));
#endif
      }
      else
      {
          std::cout << "Wait for reconnect...";
          socket_->close();
          boost::this_thread::sleep(boost::posix_time::seconds(5));
          std::cout << "Reconnecting "<<error_time<<"...";
          tcp::resolver::query query(server_, port_);
          resolver_.async_resolve(query,
              boost::bind(&client::handle_resolve, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::iterator));
      }
    }
  }

  /*void handle_read_welcome(const boost::system::error_code& error,
  size_t bytes_transferred)
  {
  if(!error)
  {
  error_time=0;
  cout.write(response,bytes_transferred);
  cout<<"\n";
  strcpy(request,"BB0202AC1203A3802013-05-11 20:06:02");
  size_t request_length = strlen(request);
  #ifdef USE_UNICODE
  request_w_length=mbstowcs(request_w,request,request_length);
  boost::asio::async_write(socket_,boost::asio::buffer(request_w,request_w_length*2),
  boost::bind(&client::handle_write_request, this,
  boost::asio::placeholders::error));
  #else
  boost::asio::async_write(socket_,boost::asio::buffer(request,request_length),
  boost::bind(&client::handle_write_request, this,
  boost::asio::placeholders::error));
  #endif
  }
  else
  {
  error_time++;
  std::cout << "Error: " << error.message() << "\n";
  if(error_time<10)
  {
  socket_.async_read_some(boost::asio::buffer(
  #ifdef USE_UNICODE
  response_w
  #else
  response
  #endif
  , max_length),
  boost::bind(&client::handle_read_welcome, this,
  boost::asio::placeholders::error,
  boost::asio::placeholders::bytes_transferred));
  }
  else
  {
  Reconnect();
  }
  }
  }*/

  void handle_read_response(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
      if(!error)
      {
          error_time=0;
#ifdef USE_UNICODE
          size_t response_length=wcstombs(response,response_w,bytes_transferred/2);
          response[response_length]='\0';
          message_process(string(response));
          //socket_.async_read_some(boost::asio::buffer(response_w, max_length),
          //    boost::bind(&client::handle_read_response, this,
          //    boost::asio::placeholders::error,
          //    boost::asio::placeholders::bytes_transferred));
#else
          message_process(string(response));
          //socket_.async_read_some(boost::asio::buffer(response, max_length),
          //    boost::bind(&client::handle_read_response, this,
          //    boost::asio::placeholders::error,
          //    boost::asio::placeholders::bytes_transferred));
#endif
          
      }
      else
      {
          error_time++;
          std::cout << "Error: " << error.message();
          if(error_time<10)
          {
              socket_->async_read_some(boost::asio::buffer(
#ifdef USE_UNICODE
                  response_w
#else
                  response
#endif
                  , max_length),
                  boost::bind(&client::handle_read_response, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
          }
          else
          {
              std::cout << "Wait for reconnect...";
              socket_->close();
              boost::this_thread::sleep(boost::posix_time::seconds(5));
              std::cout << "Reconnecting "<<error_time<<"...";
              tcp::resolver::query query(server_, port_);
              resolver_.async_resolve(query,
                  boost::bind(&client::handle_resolve, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::iterator));
          }
      }
  }

  void write_message(char* message,bool unicode=true,size_t length=0)
  {
	  try
	  {
		if(unicode)
		{
		  size_t request_length = strlen(message);
		  if(length!=0) request_length = length;
		  request_w_length=mbstowcs(request_w,message,request_length);
		  //socket_->async_write_some(boost::asio::buffer(request_w,request_w_length*2),
			 // boost::bind(&client::handle_write_request, this,
			 // boost::asio::placeholders::error));
		  boost::asio::async_write(*socket_,boost::asio::buffer(request_w,request_w_length*2),
			  boost::bind(&client::handle_write_request, this,
			  boost::asio::placeholders::error));
		}
		else
		{
		  size_t request_length = strlen(message);
		  if(length!=0) request_length=length;
		  memcpy(request,message,request_length);
		  //socket_->async_write_some(boost::asio::buffer(request,request_length),
			 // boost::bind(&client::handle_file_write_request, this,
			 // boost::asio::placeholders::error));
		  boost::asio::async_write(*socket_,boost::asio::buffer(request,request_length),
			  boost::bind(&client::handle_file_write_request, this,
			  boost::asio::placeholders::error));
		}
	  }
	  catch(...)
	  {
		  std::cout<<"WARNING:write message to server error.";
	  }
  }
  void Reconnect()
  {
      //resolver_.get_io_service().reset();
      std::cout << "Wait for reconnect...";
      socket_->close();
      boost::this_thread::sleep(boost::posix_time::seconds(5));
      std::cout << "Reconnecting "<<error_time<<"...";
      tcp::resolver::query query(server_, port_);
      resolver_.async_resolve(query,
          boost::bind(&client::handle_resolve, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::iterator));
  }
  void ReadSome()
  {
	  error_time=0;
		socket_->async_read_some(boost::asio::buffer(
#ifdef USE_UNICODE
			response_w
#else
			response
#endif
			, max_length),
			boost::bind(&client::handle_read_response, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
  }
  virtual void message_process(string message)
  {
      std::cout<<"Receive server message:"<<message;
  }
  virtual void OnConnected()
  {
      std::cout<<"Server connected.";
  }
  string server_,port_;
  tcp::resolver resolver_;
  boost::scoped_ptr<boost::asio::ip::tcp::socket> socket_;
  int error_time;
  private:
  enum { max_length = 21474836 };
  char request[max_length],response[max_length];
  wchar_t request_w[max_length],response_w[max_length];
  size_t request_w_length,response_w_length;
  boost::asio::io_service *io_service_;
};

class VIDGS_client:public client
{
public:
    VIDGS_client(boost::asio::io_service& io_service,
        const std::string& server, const std::string& port):client(io_service, server,port)
    {
		connected=false;
		current_status = PositionMaintain;
    }
    void message_process(string message);
	void send_message(int status,int faultNum=0,string send_time="");
	void send_file(string filePath);
	void OnConnected();
	bool connected;
	ClientStatus current_status;
private:
    string last_message;
};
extern VIDGS_client* glb_ptr_client;