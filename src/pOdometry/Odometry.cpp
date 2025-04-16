/************************************************************/
/*    NAME: susu                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  m_iteration = 0; //迭代次数计数器
  m_time_warp   = 0; //时间扭曲因子（用于模拟加速或减速）
  m_first_reading= 1; //标志位，表示是否是第一次读取数据
  m_current_x = 0 ; //当前时刻的位置坐标
  m_current_y = 0;
  m_previous_x = 0; //前一时刻的位置坐标
  m_previous_y = 0;
  m_total_distance = 0; //累计的总距离

  m_depth_threshold = 40;
  m_current_depth = 0;
  m_total_distance_at_depth = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key = msg.GetKey();
        
        if(key == "NAV_X") {
            m_previous_x = m_current_x;
            m_current_x = msg.GetDouble();
        }
        else if(key == "NAV_Y") {
            m_previous_y = m_current_y;
            m_current_y = msg.GetDouble();
        }
        else if(key == "NAV_DEPTH") {  // 新增深度处理
            m_current_depth = msg.GetDouble();
        }
    }
    return true;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate() {
  m_iteration++;
  double dx = m_current_x - m_previous_x;
  double dy = m_current_y - m_previous_y;
  double dist = sqrt(dx*dx + dy*dy);
  
  m_total_distance += dist; // 总里程
  Notify("ODOMETRY_DIST", m_total_distance);

  // 深度达标时累计特殊里程
  if(m_current_depth > m_depth_threshold) {
      m_total_distance_at_depth += dist;
      Notify("ODOMETRY_DIST_AT_DEPTH", m_total_distance_at_depth);
  }
  AppCastingMOOSApp::PostReport();
  return true;  // 必须返回布尔值
}
//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    
    // 读取配置参数
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        list<string>::iterator p;
        for(p=sParams.begin(); p!=sParams.end(); p++) {
            string line = *p;
            string param = toupper(biteString(line, '='));
            string value = line;
            
            if(param == "DEPTH_THRESH") {  // 读取深度阈值  //这一部分没用上，目前m_depth_threshold是写死的
                m_depth_threshold = atof(value.c_str());
                MOOSTrace("深度阈值设置为: %.1f 米\n", m_depth_threshold);
            }
        }
    }
    
    m_time_warp = GetMOOSTimeWarp();
    RegisterVariables();
    return true;
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_DEPTH", 0);
}



// AppCast报告
bool Odometry::buildReport() {
  m_msgs << "总里程: " << m_total_distance << " 米\n";
    m_msgs << "深度达标里程(" << m_depth_threshold<< "米): " 
           << m_total_distance_at_depth << " 米\n";
    return true;  // 必须返回布尔值
}