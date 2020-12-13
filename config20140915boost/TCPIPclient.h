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
    PlanePositionEmpty=1, // 01����λ�գ���λ�޷ɻ�ͣ������Ԥ������	status
    PlaneTypeSetted, // 02����Ԥ�����࣬��λ��Ԥ�����൫VIDGSû�п��� status
    VIDGSPrepared, // 03�����벶��ģʽ��VIDGS�ѿ�ʼ������û�в��񵽷ɻ�	status
    Captured, // 04�����񵽷ɻ�������λ�����Ͳ���ʱ�䣬��ʱVIDGS�ѽ�������ʶ��׶�	status
    ValidationOK, // 05��������֤�ɹ�������λ��������֤�ɹ���ʱ�䣬��ʱVIDGS���뾫ȷ�����׶�	status
    FrontWheelAligned, // 06���ɻ���һ�ζ�׼�����ߣ�����λ�����Ͷ�׼ʱ��ʱ��	status
    AccurateGuide, // 07���ɻ�����ֹͣ��12m�����͸�ʱ�̵�ʱ��	status
    GuideSuccess, // 08����ȷ�����ɹ����ɻ��ɹ�����ֹͣ�ߣ�����λ�����ͷɻ�ֹͣʱʱ��	status
    UrgencyStop, // 09������ֹͣ������ֹͣ��ť���˹����£�	status
    ValidationFail, // 10��������֤ʧ��
    AccurateGuideFail, // 11����ȷ����ʧ��
    PlaneTooFast, // 12���ɻ��ٶȹ���
    PlaneStopped, // 13���ɻ��Ѳ�λ����λ��ռ��	status
    PlaneExiting, // 14���ɻ������� status
    PositionAbandon, // 15����λ������ status
    PositionMaintain, // 16����λά���� status
    VIDGSMalfunction, // 17��VIDGS���� status
    ForeignMatter, // 18����λ�����ڻ�λ�ķɻ��������߱��ϰ������� status
    CommunicationError, // 19��ͨ�Ź��ϣ���λ����VIDGSͨ���й��� status
    Maintained, // 20��׼��������VIDGS�Ѱ�װ��ά���꣬����λ����VIDGS����ϵ�ϣ���ͣ��λδȷ����ʱʹ�� status
    ManualStart, // 21���˹�������VIDGS����Ա��ͨ������������� status
    HasNotReachStopLine, // 22����λ���̽��������ɻ�δֹͣ��ֹͣ�� status
    HasTrailer, // 23�����ϳ� status
    NoTrailer, // 24�����ϳ� status
	SysStop,  //25: ֹͣ����
    CommandConfirmed, // 26���ش�������˷���������Ϣ������Ϣ������ȷ������ִ��	
    CommandConflict, // 27���ش�������˷���������Ϣ������Ϣ���λ�ֽ׶�״̬��ͻ������ִ��
    SendFile, // 28: ��ʾVIDGSҪ����λ��������־�ļ����ļ�ΪTXT��ʽ��������������ʽ����λ�����͡�VIDGS����λ�������˴���Ϣ�󣬵ȴ�1�룬��������λ�������ļ�������
	ChockOn, // 29:�ֵ��ѷ��� status
	ChockOff, // 30:�ֵ����Ƴ� status
	ParkOn, // 31:�ֶ�ȷ�Ϸɻ���ͣ�� status
	ParkOff // 32:�ֶ�ȷ�Ϸɻ����뿪 status
};

const enum ServerStatus
{
    AskForPostionStatus=1, // 01�������λ�����λ״̬
    FlightComming, // 02���к��ཫҪ��λ
    FlightLeaving, // 03�����к��ཫҪ�뿪
    InformationChange, // 04��������Ϣ�޸ģ����絽��ʱ���޸ģ�
    FlightCanceled, // 05��ȡ������
    SetPositionAbandon, // 06����λ����
    MessageError, // 07��VIDGS������Ϣ����Ҫ���ط�
	MessageComfirm, // 08����Ϣ��Ч
	PositionAbandonCancel // 09:��λ���
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