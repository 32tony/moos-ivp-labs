/************************************************************/
/*    NAME: sy                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Underwatercomms.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef Underwatercomms_HEADER
#define Underwatercomms_HEADER
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "ProtocolHandler.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class Underwatercomms : public AppCastingMOOSApp
{
 public:
   Underwatercomms();
   ~Underwatercomms();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail) override;
   bool Iterate()override;
   bool OnConnectToServer()override;
   bool OnStartUp()override;

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport()override;

 protected:
   void registerVariables();

 private: // Configuration variables
    void startReceive();
    void handleReceive(const boost::system::error_code& error, std::size_t bytes_transferred);
    void sendData(const std::string& data);
    void runIOService();
    boost::asio::io_service m_io_service;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_local_endpoint;
    std::string local_ip;  // 本地 IP 地址
    int local_port;
    boost::asio::ip::udp::endpoint m_remote_endpoint;
    std::string target_ip;
    int target_port;
    std::array<char, 1024> m_recv_buffer;
    std::thread m_io_thread;
    ProtocolHandler m_parser;
    
 private: // State variables

};

#endif 
