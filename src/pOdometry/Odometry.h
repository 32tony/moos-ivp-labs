/************************************************************/
/*    NAME: susu                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Odometry_HEADER
#define Odometry_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class Odometry : public AppCastingMOOSApp {
public:
  Odometry();
  ~Odometry();

protected:
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  bool buildReport();

  void RegisterVariables();

protected:
  bool   m_first_reading;
  double m_current_x;
  double m_current_y;
  double m_previous_x;
  double m_previous_y;
  double m_total_distance;
  
  double m_depth_threshold;      // 新增: 深度阈值
  double m_current_depth;        // 新增: 当前深度
  double m_total_distance_at_depth; // 新增: 满足深度的里程
};

#endif