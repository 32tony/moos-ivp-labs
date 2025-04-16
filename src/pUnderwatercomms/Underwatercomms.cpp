/************************************************************/
/*    NAME: sy                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Underwatercomms.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Underwatercomms.h"
#include <boost/algorithm/hex.hpp>
using namespace std;
using boost::asio::ip::udp;

struct NAVIGATIONDATA 
{
  double longitude;
  double latitude;
};
NAVIGATIONDATA m_navigation_data;
//---------------------------------------------------------
// Constructor()

Underwatercomms::Underwatercomms():AppCastingMOOSApp(),m_socket(m_io_service)
{
}

//---------------------------------------------------------
// Destructor

Underwatercomms::~Underwatercomms()
{
  m_io_service.stop();
  if(m_io_thread.joinable())
  {
    m_io_thread.join();
  }
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool Underwatercomms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  double now = MOOSTime();
for (auto &mail : NewMail)
    {
        if(mail.IsSkewed(now))
        {
            continue;
        }
        if(mail.GetKey() == "NAV_LAT")
            m_navigation_data.latitude = mail.GetDouble();
        else if (mail.GetKey() == "NAV_LONG")
            m_navigation_data.longitude = mail.GetDouble();
    }
    return true;  //确保有返回值
}
//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool Underwatercomms::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second
bool Underwatercomms::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AcousticMessage sendmsg;
  sendmsg.type = 1;
  sendmsg.checksum = 0; // 目前未使用
  sendmsg.node_id = 28;
  sendmsg.mode = 1;
  sendmsg.retransmit = 0;
  sendmsg.control_mode = 1;
  sendmsg.send_time = static_cast<uint32_t>(time(nullptr));

  // 构造 user_data（nav_lat + nav_long）
  std::vector<uint8_t> user_data;

  union {
    double d;
    uint8_t bytes[8];
  } conv;

  conv.d = m_navigation_data.latitude;
  user_data.insert(user_data.end(), conv.bytes, conv.bytes + 8);

  conv.d = m_navigation_data.longitude;
  user_data.insert(user_data.end(), conv.bytes, conv.bytes + 8);

  sendmsg.user_data = user_data;

  // 编码并发送
  std::string hex_data = m_parser.encodeMessage(sendmsg);
  sendData(hex_data);
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Underwatercomms::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  local_ip = "192.168.1.100";  // 本地 IP 地址
  local_port = 10000;  // 本地端口号
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
   if (param == "udp_ip") {
            local_ip = value;
        } else if (param == "udp_port") {
            local_port = std::stoi(value);
            
    }
  }

     // ... 配置读取代码 ...

     try {
      // 1. 创建本地端点
      boost::asio::ip::udp::endpoint local_endpoint(
          boost::asio::ip::address::from_string(local_ip), 
          local_port
      );

      // 2. 打开并绑定Socket
      m_socket.open(boost::asio::ip::udp::v4());
      m_socket.bind(local_endpoint);

      // 3. 将m_local_endpoint设置为成员变量，用于后续日志
      m_local_endpoint = local_endpoint;
  } 
  catch (const boost::system::system_error &e) 
  {
      std::cerr << "[ERROR] 绑定失败: " << e.what() << std::endl;
      return false; // 终止启动
  }
     // 设置目标 IP 地址和端口
   std::string target_ip = "192.168.1.7";  // 目标 IP 地址
   int target_port = 8009;  // 目标端口号

    // 创建 UDP 目标端点（赋值给成员变量）
    boost::asio::ip::udp::endpoint remote_endpoint(boost::asio::ip::address::from_string(target_ip), target_port);
    m_remote_endpoint = remote_endpoint;
    // // 要发送的数据
    // std::string message = "Hello, World!";

// 发送数据包（同步发送测试消息）
// m_socket.send_to(boost::asio::buffer(message), m_remote_endpoint);
  startReceive();
  m_io_service.reset();
  m_io_thread = std::thread(&Underwatercomms::runIOService, this);
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void Underwatercomms::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_LAT", 0);
  Register("NAV_LONG", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool Underwatercomms::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}

void Underwatercomms::startReceive() {
  std::cout<<"[DEBUG]开始监听端口"<<m_local_endpoint.address()<<m_local_endpoint.port()<<std::endl;    
  m_socket.async_receive_from(
        boost::asio::buffer(m_recv_buffer), m_remote_endpoint,
        [this](const boost::system::error_code &error, std::size_t bytes_transferred) {
            handleReceive(error, bytes_transferred);
        });
}
void Underwatercomms::handleReceive(const boost::system::error_code &error, std::size_t bytes_transferred) {
    if (!error) {
        // --- 输出关键信息 ---
        std::cerr << "[DEBUG] 实际监听端口: " 
          << m_socket.local_endpoint().address().to_string() 
          << ":" << m_socket.local_endpoint().port() << std::endl;
        std::cerr << "[DEBUG] 收到数据包 (" << bytes_transferred << " 字节) "
                  << "来源: " << m_remote_endpoint.address().to_string() 
                  << ":" << m_remote_endpoint.port() << std::endl;
        std::string raw_data(m_recv_buffer.data(), bytes_transferred);
        std::string hex_data;
        // 1. 将二进制数据转为十六进制字符串（Boost实现）
        boost::algorithm::hex(raw_data, std::back_inserter(hex_data));

        // 2. ASCII转换（避免不可打印字符）
        std::string ascii_data;
        for (char c : raw_data) 
        {
            if (isprint(static_cast<unsigned char>(c))) 
            {
                ascii_data += c;
            } 
            else 
            {
                ascii_data += '.';
            }
        }

        // 3. 格式化输出（限制最大长度，避免日志爆炸）
        const size_t max_dump_len = 64;
        std::ostringstream oss;
        oss << "Hex/ASCII [前 " << std::min(bytes_transferred, max_dump_len) << " 字节]:\n"
            << hex_data.substr(0, max_dump_len * 2) << "\n"
            << ascii_data.substr(0, max_dump_len);

        std::cerr << oss.str() << std::endl;
        startReceive();  // 继续接收
    }
}
void Underwatercomms::sendData(const std::string& data) {
    std::cout << "Sending " << data.size() << " bytes to "
              << m_remote_endpoint.address().to_string() << ":"
              << m_remote_endpoint.port() << std::endl;
    m_socket.async_send_to(
        boost::asio::buffer(data), m_remote_endpoint,
        [](const boost::system::error_code& error, std::size_t bytes_sent) {
            if (!error) {
                std::cout << "Sent " << bytes_sent << " bytes successfully." << std::endl;
            } else {
                std::cerr << "Send error: " << error.message() << std::endl;
            }
        });
}

void Underwatercomms::runIOService() {
    m_io_service.run();
}


