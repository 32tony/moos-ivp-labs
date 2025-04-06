# B6_1：3：十七个Lab详解

//--------------------------------------------------------------------------------------------------------------------------------------------------------

补充一点： 当写一个自定义行为时，比如

Behavior = BHV_susu
{

}

必须要有BHV_susu.cpp/hpp做支撑



## 实验 5 - Helm Autonomy

### 5.4 任务 1 （自检） - 阿尔法回归任务

#### 目标：

此任务涉及修改 alpha 示例任务，以根据用户在 [pMarineViewer](https://oceanai.mit.edu/ivpman/apps/pMarineViewer/index.html) 窗口中的单击接受飞行器的返回航路点。您的目标是：

- 修改 alpha.moos 和 alpha.bhv（如有必要）文件，以接受用户在 [pMarineViewer](https://oceanai.mit.edu/ivpman/apps/pMarineViewer/index.html) 中单击鼠标左键来确定无人机应返回的点。您的修改应导致在用户单击后立即向 [pMarineViewer](https://oceanai.mit.edu/ivpman/apps/pMarineViewer/index.html) 窗口发布一个带有标签“return_point”的点。飞机完成航点勘测后，将前往返航点。

提示： 您将需要使用为所有行为定义的 updates 参数。返航点行为已经配置了一个 updates 参数，用于将 MOOS 变量命名为 RETURN_UPDATE。您的鼠标单击需要导致发布到此变量。此帖子的内容应为字符串，其中该字符串是行为配置块中任何配置行的直接替换。

#### 详解：

在pMarineViewer下，添加视图点标注和左键信息，

```
left_context[return-point] = RETURN_UPDATE = points=$(XPOS),$(YPOS)

left_context[return-point] = VIEW_POINT= x=$(XPOS),y=$(YPOS),label=return_point,color=red
```

在BHV_Waypoint的返回路程中，更新REYURN_UPDATE

```
updates   = RETURN_UPDATE
```

#### 代码：

```
//-------------------------------------------------
// FILE: alpha.moos
//-------------------------------------------------

ServerHost   = localhost
ServerPort   = 9000
Community    = alpha
MOOSTimeWarp = 10

// Forest Lake
LatOrigin  = 43.825300 
LongOrigin = -70.330400 

// MIT Sailing Pavilion (use this one)
// LatOrigin  = 42.358456 
// LongOrigin = -71.087589

//------------------------------------------
// Antler configuration  block
ProcessConfig = ANTLER
{
  MSBetweenLaunches = 200

  Run = MOOSDB        @ NewConsole = false
  Run = pLogger       @ NewConsole = false
  Run = uSimMarineV22 @ NewConsole = false
  Run = pMarinePIDV22 @ NewConsole = false
  Run = pHelmIvP      @ NewConsole = false
  Run = pMarineViewer	@ NewConsole = false
  Run = uProcessWatch	@ NewConsole = false
  Run = pNodeReporter	@ NewConsole = false
  Run = pRealm	      @ NewConsole = false
}

//------------------------------------------
// pLogger config block

ProcessConfig = pLogger
{
  AppTick   = 8
  CommsTick = 8

  AsyncLog = true

  // For variables that are published in a bundle on their 
  // first post, explicitly declare their logging request
  Log = IVPHELM_LIFE_EVENT @ 0 NOSYNC
  Log = REPORT @ 0 NOSYNC
  Log = BHV_SETTINGS @ 0 NOSYNC
  Log = SUMMARY @ 0 NOSYNC

  LogAuxSrc = true
  WildCardLogging = true
  WildCardOmitPattern = *_STATUS
  WildCardOmitPattern = DB_VARSUMMARY
  WildCardOmitPattern = DB_RWSUMMARY
  WildCardExclusionLog = true
}

//------------------------------------------
// uProcessWatch config block

ProcessConfig = uProcessWatch
{
  AppTick   = 4
  CommsTick = 4

  watch_all = true
	nowatch   = uPokeDB*
	nowatch   = uQueryDB*
	nowatch   = uXMS*
	nowatch   = uMAC*
}

//------------------------------------------
// uSimMarineV22 config block

ProcessConfig = uSimMarineV22
{
  AppTick  	= 4
  CommsTick	= 4

  start_pos     = x=0, y=-20, heading=180, speed=0
  prefix        = NAV
  turn_rate     = 95
  thrust_map    = 0:0, 20:1, 40:2, 60:3, 80:4, 100:5
	turn_spd_loss = 0.4
}

//------------------------------------------
// pHelmIvP config block

ProcessConfig = pHelmIvP
{
  AppTick    = 4
  CommsTick  = 4

  behaviors  = alpha.bhv  //<-------------------------------------
  domain     = course:0:359:360
  domain     = speed:0:4:41
}

//------------------------------------------
// pMarinePID config block

ProcessConfig = pMarinePIDV22
{
  AppTick    = 20
  CommsTick  = 20

  verbose       = true
  depth_control = false

  // SIM_INSTABILITY = 20

  // Yaw PID controller
  yaw_pid_kp		 = 0.9
  yaw_pid_kd		 = 0.3
  yaw_pid_ki		 = 0.3
  yaw_pid_integral_limit = 0.07

  // Speed PID controller
  speed_pid_kp		 = 1.0
  speed_pid_kd		 = 0.0
  speed_pid_ki		 = 0.0
  speed_pid_integral_limit = 0.07

  //MAXIMUMS
  maxrudder    = 100
  maxthrust    = 100

  // A non-zero SPEED_FACTOR overrides use of SPEED_PID
  // Will set DESIRED_THRUST = DESIRED_SPEED * SPEED_FACTOR
  speed_factor = 20
}

//------------------------------------------
// pMarineViewer config block

ProcessConfig = pMarineViewer
{
  AppTick    = 4
  CommsTick  = 4

  tiff_file            = forrest19.tif
	//tiff_file            = MIT_SP.tif

  set_pan_x            = -90
  set_pan_y            = -280
  zoom                 = 0.65
  vehicle_shape_scale  = 1.5
  hash_delta           = 50
  hash_shade           = 0.22
  hash_viewable        = true

  trails_point_size   = 1

	//op_vertex = x=-83, y=-47,    lwidth=1,lcolor=yellow,looped=true,group=moa

  //op_vertex = x=-46.4, y=-129.2, lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=94.6, y=-62.2,   lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=58,   y=20,    lwidth=1,lcolor=yellow,looped=true,group=moa

  // Appcast configuration
  appcast_height       = 75
  appcast_width        = 30
  appcast_viewable     = true
  appcast_color_scheme = indigo
  nodes_font_size      = xlarge
  procs_font_size      = xlarge
  appcast_font_size    = large

  // datum_viewable = true
  // datum_size     = 18
  // gui_size = small

  // left_context[survey-point] = DEPLOY=true
  // left_context[survey-point] = MOOS_MANUAL_OVERRIDE=false
  // left_context[survey-point] = RETURN=false
    left_context[return-point] = RETURN_UPDATE = points=$(XPOS),$(YPOS)
  left_context[return-point] = VIEW_POINT = x=$(XPOS),y=$(YPOS),label=return_point,color=red


  right_context[return] = DEPLOY=true
  right_context[return] = MOOS_MANUAL_OVERRIDE=false
  right_context[return] = RETURN=false

  scope  = SUMMARY
  scope  = RETURN
  scope  = WPT_STAT
  scope  = VIEW_SEGLIST              //这个scope只是在图形窗口中，下拉看变量的，不影响行为和helm
  scope  = VIEW_POINT
  scope  = VIEW_POLYGON
  scope  = MVIEWER_LCLICK
  scope  = MVIEWER_RCLICK

  button_one = DEPLOY # DEPLOY=true
  button_one = MOOS_MANUAL_OVERRIDE=false # RETURN=false
  button_two = RETURN # RETURN=true
  button_three = SLOWER # WPT_UPDATE=speed=1.5
  button_four = FASTER # WPT_UPDATE=speed=3.5

  action  = MENU_KEY=deploy # DEPLOY = true # RETURN = false
  action+ = MENU_KEY=deploy # MOOS_MANUAL_OVERRIDE=false
  action  = RETURN=true
  action  = UPDATES_RETURN=speed=1.4
}

//------------------------------------------
// pNodeReporter config block

ProcessConfig = pNodeReporter
{
  AppTick    = 2
  CommsTick	 = 2

  platform_type   = kayak
  platform_color  = yellow
  platform_length = 4
}
```

////////////////////

```
//--------    FILE: alpha.bhv   -------------

initialize   DEPLOY = false
initialize   RETURN = false
	
//----------------------------------------------
Behavior = BHV_Waypoint
{ 
  name      = waypt_survey
  pwt       = 100
  condition = RETURN = false
  condition = DEPLOY = true
  endflag   = RETURN = true

	endflag   = SUMMARY=speed=$[SPEED],time=$[DUR_RUN_TIME]
	
	configflag = CRUISE_SPD = $[SPEED]
	
	activeflag = INFO=$[OWNSHIP]
	activeflag = INFO=$[BHVNAME]
	activeflag = INFO=$[BHVTYPE]
	
	cycleflag = CINFO=$[OSX],$[OSY]
	
	wptflag = PREV=$(PX),$(PY)
	wptflag = NEXT=$(NX),$(NY)
	wptflag = TEST=$(X),$(Y)
	wptflag = OSPOS=$(OSX),$(OSY)
	wptflag_on_start = true

  updates    = WPT_UPDATE
  perpetual  = true

	         speed_alt = 1.2
	   use_alt_speed = true
	            lead = 8
	     lead_damper = 1
	   lead_to_start = true
	           speed = 3   // meters per second
	    capture_line = true
	  capture_radius = 5.0
	     slip_radius = 15.0
	efficiency_measure = all
	           polygon = 60,-40:60,-160:150,-160:180,-100:150,-40
	
	           order = normal
	         repeat  = 0
	
	visual_hints = nextpt_color=yellow
	visual_hints = nextpt_vertex_size=8
	visual_hints = nextpt_lcolor=gray70
	visual_hints = vertex_color=dodger_blue, edge_color=white
	visual_hints = vertex_size=5, edge_size=1

}

//----------------------------------------------
Behavior=BHV_Waypoint
{
  name      = waypt_return
  pwt       = 100
  condition = RETURN = true
  condition = DEPLOY = true
  perpetual = true
  updates   = RETURN_UPDATE//<-------------------------------------
  endflag   = RETURN = false
  endflag   = DEPLOY = false
  endflag   = MISSION = complete
	

           speed = 2.0

  capture_radius = 2.0
     slip_radius = 8.0
          points = 0,-20
 }

//----------------------------------------------
Behavior=BHV_ConstantSpeed
{
  name      = const_speed
  pwt       = 200
  condition = SPD=true
  condition = DEPLOY = true
  perpetual = true
  updates   = SPEED_UPDATE
  endflag   = SPD = false

           speed = 0.5
    	    duration = 10
    duration_reset = CONST_SPD_RESET=true

}
```

### 5.5 任务 2 （勾选） - Alpha 立即回归任务

#### 目标：

  将您的任务和 [pMarineViewer](https://oceanai.mit.edu/ivpman/apps/pMarineViewer/index.html) 配置为具有第二个鼠标左键单击上下文，指示无人机在单击鼠标后立即返回。无人机不是等到航路点调查行为完成，而是单击鼠标左键导致立即返回指定点。

#### 详解：

删除pMarineViewer下的视图点标注和左键信息

添加左键立即返回命令：

```
 left_context[return-point] = RETURN = true 
```

#### 代码：

同上，微调，删除两行，加入一行



### 6.1 任务 3 （自检） - Bravo Loiter 任务

#### 目标：

第一步是创建一个新的任务和行为文件，并将它们命名为 bravo.moos 和 bravo.bhv。这应该放在 missions/lab_05/ 中一个名为 bravo_loiter 的文件夹中。复制 alpha 任务作为起点当然是可以的。bravo 任务应配置以下功能：

- 它应该具有 Loiter 行为，该行为主要在初始部署时处于活动状态。它的位置应为 x=100，y=-75，半径为 30 米，并且 loiter 多边形应有 8 个顶点。它应设置为 2.5 m/sec 的悬停速度。它应逆时针徘徊。
- 它应该有一个航路点行为，当变量 RETURN=true 时，它只是将无人机返回到无人机起始位置 x=0，y=0，就像在 alpha 任务中一样。
- Loiter 行为应利用 duration 参数在 150 秒后自动“完成”，从而触发返回航点行为

#### 详解：

可把alpha的源文件拿过来修改，修改参考如下：

Loiter 行为在 **helm 文档** （http://oceanai.mit.edu/ivpman/bhvs/Loiter） 中。此处需要注意的主要参数是 polygon、clockwise 和 speed 参数。使用 format=radial 格式来描述悬停多边形，如上面引用的 Loiter 文档的“设置和更改 Loiter 区域”部分所述。

**航点返回行为的配置**与 Alpha 任务类似，但有关此行为的更多信息，请参阅 http://oceanai.mit.edu/ivpman/bhv

**持续时间**行为参数是为所有行为定义的。如果未指定，则大多数行为将持续时间视为无限。有关此参数的更详细讨论，请参阅 helm 文档：http://oceanai.mit.edu/ivpman/helm/param_duration。

我们在本次任务和以下任务中探索行为 run-state 和 run-flags 的概念。可以在此处找到对此的精彩讨论：http://oceanai.mit.edu/ivpman/helm/run_states。

**1、配置Loiter（在.bhv中），在helm文档中，提供了示例配置块**

```
//示例配置块
Behavior = BHV_Loiter
 {
   // General Behavior Parameters
   // ---------------------------
   name         = transit               // example
   pwt          = 100                   // default
   condition    = MODE==LOITERING       // example
   updates      = LOITER_UPDATES        // example

   // Parameters specific to this behavior
   // ------------------------------------
        acquire_dist = 10               // default
      capture_radius = 3                // default  
     center_activate = false            // default
           clockwise = true             // default
         slip_radius = 15               // default
               speed = 0                // default
       spiral_factor = -2               // default

             polygon = radial:: x=5,y=8,radius=20,pts=8  // example
         post_suffix = HENRY                             // example

       center_assign = 40,50            // example
      xcenter_assign = 40               // example
      ycenter_assign = 50               // example


        visual_hints = vertex_size  = 1             // default
        visual_hints = edge_size    = 1             // default
        visual_hints = vertex_color = dodger_blue   // default
        visual_hints = edge_color   = white         // default
        visual_hints = nextpt_color = yellow        // default
        visual_hints = nextpt_lcolor = aqua         // default
        visual_hints = nextpt_vertex_size = 5       // default
        visual_hints = label        = zone3         // example
 }
```

根据目标要求：

- 它应该具有 Loiter 行为，该行为主要在初始部署时处于活动状态。它的位置应为 x=100，y=-75，半径为 30 米，并且 loiter 多边形应有 8 个顶点。它应设置为 2.5 m/sec 的悬停速度。它应逆时针徘徊。
- 它应该有一个航路点行为，当变量 RETURN=true 时，它只是将无人机返回到无人机起始位置 x=0，y=0，就像在 alpha 任务中一样。
- Loiter 行为应利用 duration 参数在 150 秒后自动“完成”，从而触发返回航点行为

**对要求进行拆解：**

（1）该行为主要在初始部署时处于活动状态

```
  condition = DEPLOY = true
  condition = RETURN = false 
  endflag = RETURN = true
```

（2）它的位置应为 x=100，y=-75，半径为 30 米，并且 loiter 多边形应有 8 个顶点

```
 polygon = radial:: x=100,y=-75,radius=30,pts=8 
```

（3）它应设置为 2.5 m/sec 的悬停速度

```
 speed = 2.5   
```

（4）它应逆时针徘徊

```
 clockwise = false
```

（5）它应该有一个航路点行为，当变量 RETURN=true 时，它只是将无人机返回到无人机起始位置 x=0，y=0，就像在 alpha 任务中一样

​              Behavior=BHV_Waypoint中设

```
points = 0,0
```

（6）Loiter 行为应利用 duration 参数在 150 秒后自动“完成”，从而触发返回航点行为 <-----helm文档中持续时间的配置

```
  duration = 150  //行为执行150s后，行为结束
  updates    = LOITER_UPDATE
  perpetual  = true
```

**2.配置pHelmIvP（在.moos中）**

```
ProcessConfig = pHelmIvP
{
  AppTick    = 4
  CommsTick  = 4

  behaviors  = bravo.bhv  //<------加入.bhv行为
  domain     = course:0:359:360
  domain     = speed:0:4:41
}
```

其他不用改

#### 代码：

```
///-------------------------------------------------
// FILE: alpha.moos
//-------------------------------------------------

ServerHost   = localhost
ServerPort   = 9000
Community    = alpha
MOOSTimeWarp = 10

// Forest Lake
LatOrigin  = 43.825300 
LongOrigin = -70.330400 

// MIT Sailing Pavilion (use this one)
// LatOrigin  = 42.358456 
// LongOrigin = -71.087589

//------------------------------------------
// Antler configuration  block
ProcessConfig = ANTLER
{
  MSBetweenLaunches = 200

  Run = MOOSDB        @ NewConsole = false
  Run = pLogger       @ NewConsole = false
  Run = uSimMarineV22 @ NewConsole = false
  Run = pMarinePIDV22 @ NewConsole = false
  Run = pHelmIvP      @ NewConsole = false
  Run = pMarineViewer	@ NewConsole = false
  Run = uProcessWatch	@ NewConsole = false
  Run = pNodeReporter	@ NewConsole = false
  Run = pRealm	      @ NewConsole = false
}

//------------------------------------------
// pLogger config block

ProcessConfig = pLogger
{
  AppTick   = 8
  CommsTick = 8

  AsyncLog = true

  // For variables that are published in a bundle on their 
  // first post, explicitly declare their logging request
  Log = IVPHELM_LIFE_EVENT @ 0 NOSYNC
  Log = REPORT @ 0 NOSYNC
  Log = BHV_SETTINGS @ 0 NOSYNC
  Log = SUMMARY @ 0 NOSYNC

  LogAuxSrc = true
  WildCardLogging = true
  WildCardOmitPattern = *_STATUS
  WildCardOmitPattern = DB_VARSUMMARY
  WildCardOmitPattern = DB_RWSUMMARY
  WildCardExclusionLog = true
}

//------------------------------------------
// uProcessWatch config block

ProcessConfig = uProcessWatch
{
  AppTick   = 4
  CommsTick = 4

  watch_all = true
	nowatch   = uPokeDB*
	nowatch   = uQueryDB*
	nowatch   = uXMS*
	nowatch   = uMAC*
}

//------------------------------------------
// uSimMarineV22 config block

ProcessConfig = uSimMarineV22
{
  AppTick  	= 4
  CommsTick	= 4

  start_pos     = x=0, y=-20, heading=180, speed=0
  prefix        = NAV
  turn_rate     = 95
  thrust_map    = 0:0, 20:1, 40:2, 60:3, 80:4, 100:5
	turn_spd_loss = 0.4
}

//------------------------------------------
// pHelmIvP config block

ProcessConfig = pHelmIvP
{
  AppTick    = 4
  CommsTick  = 4

  behaviors  = bravo.bhv
  domain     = course:0:359:360
  domain     = speed:0:4:41
}

//------------------------------------------
// pMarinePID config block

ProcessConfig = pMarinePIDV22
{
  AppTick    = 20
  CommsTick  = 20

  verbose       = true
  depth_control = false

  // SIM_INSTABILITY = 20

  // Yaw PID controller
  yaw_pid_kp		 = 0.9
  yaw_pid_kd		 = 0.3
  yaw_pid_ki		 = 0.3
  yaw_pid_integral_limit = 0.07

  // Speed PID controller
  speed_pid_kp		 = 1.0
  speed_pid_kd		 = 0.0
  speed_pid_ki		 = 0.0
  speed_pid_integral_limit = 0.07

  //MAXIMUMS
  maxrudder    = 100
  maxthrust    = 100

  // A non-zero SPEED_FACTOR overrides use of SPEED_PID
  // Will set DESIRED_THRUST = DESIRED_SPEED * SPEED_FACTOR
  speed_factor = 20
}

//------------------------------------------
// pMarineViewer config block

ProcessConfig = pMarineViewer
{
  AppTick    = 4
  CommsTick  = 4

  tiff_file            = forrest19.tif
	//tiff_file            = MIT_SP.tif

  set_pan_x            = -90
  set_pan_y            = -280
  zoom                 = 0.65
  vehicle_shape_scale  = 1.5
  hash_delta           = 50
  hash_shade           = 0.22
  hash_viewable        = true

  trails_point_size   = 1

	//op_vertex = x=-83, y=-47,    lwidth=1,lcolor=yellow,looped=true,group=moa

  //op_vertex = x=-46.4, y=-129.2, lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=94.6, y=-62.2,   lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=58,   y=20,    lwidth=1,lcolor=yellow,looped=true,group=moa

  // Appcast configuration
  appcast_height       = 75
  appcast_width        = 30
  appcast_viewable     = true
  appcast_color_scheme = indigo
  nodes_font_size      = xlarge
  procs_font_size      = xlarge
  appcast_font_size    = large

  // datum_viewable = true
  // datum_size     = 18
  // gui_size = small

  // left_context[survey-point] = DEPLOY=true
  // left_context[survey-point] = MOOS_MANUAL_OVERRIDE=false
  // left_context[survey-point] = RETURN=false

   // left_context[return-point] = RETURN_UPDATE = points=$(XPOS),$(YPOS)
 // left_context[return-point] = VIEW_POINT = x=$(XPOS),y=$(YPOS),label=return_point,color=red


  right_context[return] = DEPLOY=true
  right_context[return] = MOOS_MANUAL_OVERRIDE=false
  right_context[return] = RETURN=false

  scope  = SUMMARY
  scope  = RETURN
  scope  = WPT_STAT
  scope  = VIEW_SEGLIST
  scope  = VIEW_POINT
  scope  = VIEW_POLYGON
  scope  = MVIEWER_LCLICK
  scope  = MVIEWER_RCLICK

  button_one = DEPLOY # DEPLOY=true
  button_one = MOOS_MANUAL_OVERRIDE=false # RETURN=false
  button_two = RETURN # RETURN=true
  button_three = SLOWER # WPT_UPDATE=speed=1.5
  button_four = FASTER # WPT_UPDATE=speed=3.5

  action  = MENU_KEY=deploy # DEPLOY = true # RETURN = false
  action+ = MENU_KEY=deploy # MOOS_MANUAL_OVERRIDE=false
  action  = RETURN=true
  action  = UPDATES_RETURN=speed=1.4
}

//------------------------------------------
// pNodeReporter config block

ProcessConfig = pNodeReporter
{
  AppTick    = 2
  CommsTick	 = 2

  platform_type   = kayak
  platform_color  = yellow
  platform_length = 4
}
```

////////////////////////////////////

```
///--------    FILE: alpha.bhv   -------------

initialize   DEPLOY = false
initialize   RETURN = false
	
//----------------------------------------------
Behavior = BHV_Waypoint
{ 
  name      = waypt_survey
  pwt       = 100
  condition = RETURN = false
  condition = DEPLOY = true
  endflag   = RETURN = true

	endflag   = SUMMARY=speed=$[SPEED],time=$[DUR_RUN_TIME]
	
	configflag = CRUISE_SPD = $[SPEED]
	
	activeflag = INFO=$[OWNSHIP]
	activeflag = INFO=$[BHVNAME]
	activeflag = INFO=$[BHVTYPE]
	
	cycleflag = CINFO=$[OSX],$[OSY]
	
	wptflag = PREV=$(PX),$(PY)
	wptflag = NEXT=$(NX),$(NY)
	wptflag = TEST=$(X),$(Y)
	wptflag = OSPOS=$(OSX),$(OSY)
	wptflag_on_start = true

  updates    = WPT_UPDATE
  perpetual  = true

	         speed_alt = 1.2
	   use_alt_speed = true
	            lead = 8
	     lead_damper = 1
	   lead_to_start = true
	           speed = 3   // meters per second
	    capture_line = true
	  capture_radius = 5.0
	     slip_radius = 15.0
	efficiency_measure = all
	           //polygon = 60,-40:60,-160:150,-160:180,-100:150,-40
	           //polygon = format=radial, x=75, y=75, radius=40, pts=6, snap=1, label=Lima
	
	           order = normal
	         repeat  = 0
	
	visual_hints = nextpt_color=yellow
	visual_hints = nextpt_vertex_size=8
	visual_hints = nextpt_lcolor=gray70
	visual_hints = vertex_color=dodger_blue, edge_color=white
	visual_hints = vertex_size=5, edge_size=1

}

//----------------------------------------------
Behavior=BHV_Waypoint
{
  name      = waypt_return
  pwt       = 100
  condition = RETURN = true
  condition = DEPLOY = true
  perpetual = true
  updates   = RETURN_UPDATE
  endflag   = RETURN = false
  endflag   = DEPLOY = false
  endflag   = MISSION = complete
	

           speed = 2.0

  capture_radius = 2.0
     slip_radius = 8.0
          points = 0,0
 }
//-----------------------------------------------------
 Behavior = BHV_Loiter
 {
   // General Behavior Parameters
   // ---------------------------
   name         = west_zone// example
   pwt          = 100                   // default
   //condition    = MODE==LOITERING       // example
          // example

    condition = DEPLOY = true

  condition = RETURN = false   //这三行要注意，启动的条件
  endflag = RETURN = true

  duration = 150  //行为执行150s后，行为结束
  updates    = LOITER_UPDATE
  perpetual  = true

   // Parameters specific to this behavior
   // ------------------------------------
        acquire_dist = 10               // default
      capture_radius = 3                // default  
     center_activate = false            // default
           clockwise = true             // default
         slip_radius = 15               // default
               speed = 2.5                // default
       spiral_factor = -2               // default
       

          // center_assign = 40,50            // example
    // xcenter_assign = 40               // example
     // ycenter_assign = 50               // example
    //上三行可覆盖polygon，形成新的目标中心
    
              polygon = radial:: x=100,y=-75,radius=30,pts=8 
         post_suffix = HENRY                             // example


        visual_hints = vertex_size  = 1             // default
        visual_hints = edge_size    = 1             // default
        visual_hints = vertex_color = dodger_blue   // default
        visual_hints = edge_color   = white         // default
        visual_hints = nextpt_color = yellow        // default
        visual_hints = nextpt_lcolor = aqua         // default
        visual_hints = nextpt_vertex_size = 5       // default
        visual_hints = label        = west_zone         // example

 }

//----------------------------------------------
Behavior=BHV_ConstantSpeed
{
  name      = const_speed
  pwt       = 200
  condition = SPD=true
  condition = DEPLOY = true
  perpetual = true
  updates   = SPEED_UPDATE
  endflag   = SPD = false

           speed = 0.5
    	    duration = 10
    duration_reset = CONST_SPD_RESET=true

}
```



### 6.2 任务 4 （自检） - Bravo Double Loiter 任务

#### 目标：

将第二个 loiter 行为添加到 bravo 任务中。这个想法是构建一个任务，让无人机能够在两个位置的徘徊之间定期切换。我们将尝试各种方法来启用这种切换，既可以自动切换，也可以通过用户交互来实现。第二个 bravo 任务应配置以下功能：

- 复制上一个 bravo 任务，在 missions/lab_05/ 文件夹中创建一个名为 bravo_loiter_dbl 的新目录。
- 新的 bravo 任务应具有第二个悬停行为，该行为在初始部署时未处于活动状态。它的位置应为 x=160，y=-50，半径为 20 米，并且悬停多边形应具有 8 个顶点。应将其设置为悬停速度 2.5 m/sec。
- 使用它们的 condition 参数使两个 loiter 行为互斥，例如 condition=LOITER_REGION=west。
- 利用 duration、endflag 和 perpetual 参数来完成 loiter 区域之间的周期性切换。将每个 duration 设置为 150 秒，在每个行为中使用几个 endflag 参数来 （a） 触发另一个 luiter 行为的条件，以及 （b） 否定行为结束的条件。将 perpetual 参数设置为 true，以便完成的行为不会永久完成，而只是等待其条件再次得到满足。
- **注意：** 你可能需要了解持续时间计时器是如何工作的。回想一下，持续时间功能是为所有行为定义的。默认情况下，一旦它完全倒计时并且 perpetual 设置为 true，计时器就会立即重新启动。这可能不是你想要的。你可能希望你的行为等到它再次运行（满足逻辑条件）后再恢复持续时间计时器倒计时。在这种情况下，你需要将 duration_idle_decay 设置为 false。默认值为 true

#### 详解：

双任务切换，注意，双任务，有两个Behavior（Behavior = BHV_Loiter），这两个行为切换，需要用条件去切换。还有一个注意的地方，即最开的条件的触发问题，下面会说到

**对要求进行拆解：**

注意，这个任务4是任务3的变型，所有要在任务3的基础上继续写，把代码复制过来，我们来把它改成符合任务4的代码

（1）：新的 bravo 任务应具有第二个悬停行为：我们把任务3中的.bhv中的Behavior = BHV_Loiter完整的复制过来，粘贴在它下面，这样就有两个完全一样的Behavior = BHV_Loiter了，下面开始修改

（2）：为了区分两个Behavior = BHV_Loiter，修改其name量，一个叫loiter_west，另一个叫loiter_east，这就是两个悬停任务的名称
（3）：好了，第一个悬停先不动

（4）：第二个悬停，该行为在初始部署时未处于活动状态，这里要和要求“使用它们的 condition 参数使两个 loiter 行为互斥，例如 condition=LOITER_REGION=west。”结合写，让他无法满足要求，就会处于非活动状态

```
   condition = RETURN = false
   condition = DEPLOY = true
   condition = LOITER_REGION=east
```

（5）：它的位置应为 x=160，y=-50，半径为 20 米，并且悬停多边形应具有 8 个顶点

```
polygon = radial:: x=160,y=-50,radius=20,pts=8 
```

（6）：应将其设置为悬停速度 2.5 m/sec

```
speed = 2.5  
```

（7）：使用它们的 condition 参数使两个 loiter 行为互斥，例如 condition=LOITER_REGION=west

这里为了让行为互斥，要在两个悬停中，加入相反的触发条件去限制它

**第一个悬停：**

```
condition = LOITER_REGION = west

endflag = LOITER_REGION = east
```

**第二个悬停：**

```
condition = LOITER_REGION=east

endflag = LOITER_REGION = west
```

（8）利用 duration、endflag 和 perpetual 参数来完成 loiter 区域之间的周期性切换。将每个 duration 设置为 150 秒，在每个行为中使用几个 endflag 参数来 （a） 触发另一个 luiter 行为的条件，以及 （b） 否定行为结束的条件，将 perpetual 参数设置为 true，以便完成的行为不会永久完成，而只是等待其条件再次得到满足。

这一个要求包括上面（7），下面补充一个完整的

**第一个悬停：**

```
   condition = DEPLOY = true
   condition = RETURN = false   
   condition = LOITER_REGION = west
   updates      = LOITER_UPDATE
   duration = 150
   endflag = LOITER_REGION = east  
   perpetual  = true
```

**第二个悬停：**

```
   condition = RETURN = false
   condition = DEPLOY = true
   condition = LOITER_REGION=east
   updates      = LOITER_UPDATE  
   duration = 150
   endflag = LOITER_REGION = west
   perpetual  = true
```

（9）：**注意：** 你可能需要了解持续时间计时器是如何工作的。回想一下，持续时间功能是为所有行为定义的。默认情况下，一旦它完全倒计时并且 perpetual 设置为 true，计时器就会立即重新启动。这可能不是你想要的。你可能希望你的行为等到它再次运行（满足逻辑条件）后再恢复持续时间计时器倒计时。在这种情况下，你需要将 duration_idle_decay 设置为 false。默认值为 true

```
duration_idle_decay = false  //<-------行为的持续时间时钟将保持暂停状态，直到满足其条件
```

（10）：补充说明

上面提到的updates、duration、perpetual、polygon等，在helm文档中全由说明，条件限制及行为 run-state 和 run-flags 的概念在http://oceanai.mit.edu/ivpman/helm/run_states中

（11）：最初的条件condition是怎么触发的，在pMarineViewer画面中，点击部署DEPLOY按钮时触发LOITER_REGION = west，具体的写法，全部放在代码中。

```
button_one = DEPLOY # DEPLOY=true # LOITER_REGION = west
```

#### 代码：

```
///-------------------------------------------------
// FILE: bravo.moos
//-------------------------------------------------

ServerHost   = localhost
ServerPort   = 9000
Community    = bravo
MOOSTimeWarp = 10


// MIT Sailing Pavilion (use this one)
 LatOrigin  = 42.358456 
 LongOrigin = -71.087589

//------------------------------------------
// Antler configuration  block
ProcessConfig = ANTLER
{
  MSBetweenLaunches = 200

  Run = MOOSDB        @ NewConsole = false
  Run = pLogger       @ NewConsole = false
  Run = uSimMarineV22 @ NewConsole = false
  Run = pMarinePIDV22 @ NewConsole = false
  Run = pHelmIvP      @ NewConsole = false
  Run = pMarineViewer	@ NewConsole = false
  Run = uProcessWatch	@ NewConsole = false
  Run = pNodeReporter	@ NewConsole = false
  Run = pRealm	      @ NewConsole = false
}

//------------------------------------------
// pLogger config block

ProcessConfig = pLogger
{
  AppTick   = 8
  CommsTick = 8

  AsyncLog = true

  // For variables that are published in a bundle on their 
  // first post, explicitly declare their logging request
  Log = IVPHELM_LIFE_EVENT @ 0 NOSYNC
  Log = REPORT @ 0 NOSYNC
  Log = BHV_SETTINGS @ 0 NOSYNC
  Log = SUMMARY @ 0 NOSYNC

  LogAuxSrc = true
  WildCardLogging = true
  WildCardOmitPattern = *_STATUS
  WildCardOmitPattern = DB_VARSUMMARY
  WildCardOmitPattern = DB_RWSUMMARY
  WildCardExclusionLog = true
}

//------------------------------------------
// uProcessWatch config block

ProcessConfig = uProcessWatch
{
  AppTick   = 4
  CommsTick = 4

  watch_all = true
	nowatch   = uPokeDB*
	nowatch   = uQueryDB*
	nowatch   = uXMS*
	nowatch   = uMAC*
}

//------------------------------------------
// uSimMarineV22 config block

ProcessConfig = uSimMarineV22
{
  AppTick  	= 4
  CommsTick	= 4

  start_pos     = x=0, y=0, heading=180, speed=0
  prefix        = NAV
  turn_rate     = 95
  thrust_map    = 0:0, 20:1, 40:2, 60:3, 80:4, 100:5
	turn_spd_loss = 0.4
}

//------------------------------------------
// pHelmIvP config block

ProcessConfig = pHelmIvP
{
  AppTick    = 4
  CommsTick  = 4

  behaviors  = bravo.bhv
  domain     = course:0:359:360
  domain     = speed:0:4:41
}

//------------------------------------------
// pMarinePID config block

ProcessConfig = pMarinePIDV22
{
  AppTick    = 20
  CommsTick  = 20

  verbose       = true
  depth_control = false

  // SIM_INSTABILITY = 20

  // Yaw PID controller
  yaw_pid_kp		 = 0.9
  yaw_pid_kd		 = 0.3
  yaw_pid_ki		 = 0.3
  yaw_pid_integral_limit = 0.07

  // Speed PID controller
  speed_pid_kp		 = 1.0
  speed_pid_kd		 = 0.0
  speed_pid_ki		 = 0.0
  speed_pid_integral_limit = 0.07

  //MAXIMUMS
  maxrudder    = 100
  maxthrust    = 100

  // A non-zero SPEED_FACTOR overrides use of SPEED_PID
  // Will set DESIRED_THRUST = DESIRED_SPEED * SPEED_FACTOR
  speed_factor = 20
}

//------------------------------------------
// pMarineViewer config block

ProcessConfig = pMarineViewer
{
  AppTick    = 4
  CommsTick  = 4

  tiff_file            = forrest19.tif
	//tiff_file            = MIT_SP.tif

  set_pan_x            = -90
  set_pan_y            = -280
  zoom                 = 0.65
  vehicle_shape_scale  = 1.5
  hash_delta           = 50
  hash_shade           = 0.22
  hash_viewable        = true

  trails_point_size   = 1

	//op_vertex = x=-83, y=-47,    lwidth=1,lcolor=yellow,looped=true,group=moa

  //op_vertex = x=-46.4, y=-129.2, lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=94.6, y=-62.2,   lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=58,   y=20,    lwidth=1,lcolor=yellow,looped=true,group=moa

  // Appcast configuration
  appcast_height       = 75
  appcast_width        = 30
  appcast_viewable     = true
  appcast_color_scheme = indigo
  nodes_font_size      = xlarge
  procs_font_size      = xlarge
  appcast_font_size    = large

  // datum_viewable = true
  // datum_size     = 18
  // gui_size = small

  // left_context[survey-point] = DEPLOY=true
  // left_context[survey-point] = MOOS_MANUAL_OVERRIDE=false
  // left_context[survey-point] = RETURN=false

   // left_context[return-point] = RETURN_UPDATE = points=$(XPOS),$(YPOS)
 // left_context[return-point] = VIEW_POINT = x=$(XPOS),y=$(YPOS),label=return_point,color=red


  right_context[return] = DEPLOY=true
  right_context[return] = MOOS_MANUAL_OVERRIDE=false
  right_context[return] = RETURN=false

  scope  = SUMMARY
  scope  = RETURN
  scope  = WPT_STAT
  scope  = VIEW_SEGLIST
  scope  = VIEW_POINT
  scope  = VIEW_POLYGON
  scope  = MVIEWER_LCLICK
  scope  = MVIEWER_RCLICK
  scope  = LOITER_REGION
  scope  = LOITER

  button_one = DEPLOY # DEPLOY=true # LOITER_REGION = west
  button_one = MOOS_MANUAL_OVERRIDE=false # RETURN=false
  button_two = RETURN # RETURN=true
  button_three = SLOWER # WPT_UPDATE=speed=1.5
  button_four = FASTER # WPT_UPDATE=speed=3.5

  action  = MENU_KEY=deploy # DEPLOY = true # RETURN = false
  action+ = MENU_KEY=deploy # MOOS_MANUAL_OVERRIDE=false
  action  = RETURN=true
  action  = UPDATES_RETURN=speed=1.4
}

//------------------------------------------
// pNodeReporter config block

ProcessConfig = pNodeReporter
{
  AppTick    = 2
  CommsTick	 = 2

  platform_type   = kayak
  platform_color  = yellow
  platform_length = 4
}
```

////////////////////////

```
///--------    FILE: bravo.bhv   -------------

initialize   DEPLOY = false
initialize   RETURN = false
	
//----------------------------------------------
Behavior = BHV_Waypoint
{ 
  name      = waypt_survey
  pwt       = 100
  condition = RETURN = false
  condition = DEPLOY = true
  //condition = LOITER_REGION = west
  endflag   = RETURN = true

	endflag   = SUMMARY=speed=$[SPEED],time=$[DUR_RUN_TIME]
	
	configflag = CRUISE_SPD = $[SPEED]
	
	activeflag = INFO=$[OWNSHIP]
	activeflag = INFO=$[BHVNAME]
	activeflag = INFO=$[BHVTYPE]
	
	cycleflag = CINFO=$[OSX],$[OSY]
	
	wptflag = PREV=$(PX),$(PY)
	wptflag = NEXT=$(NX),$(NY)
	wptflag = TEST=$(X),$(Y)
	wptflag = OSPOS=$(OSX),$(OSY)
	wptflag_on_start = true

  updates    = WPT_UPDATE
  perpetual  = true

	         speed_alt = 1.2
	   use_alt_speed = true
	            lead = 8
	     lead_damper = 1
	   lead_to_start = true
	           speed = 3   // meters per second
	    capture_line = true
	  capture_radius = 5.0
	     slip_radius = 15.0
	efficiency_measure = all
	
	           order = normal
	         repeat  = 0
	    
	visual_hints = nextpt_color=yellow
	visual_hints = nextpt_vertex_size=8
	visual_hints = nextpt_lcolor=gray70
	visual_hints = vertex_color=dodger_blue, edge_color=white
	visual_hints = vertex_size=5, edge_size=1

}

//----------------------------------------------
Behavior=BHV_Waypoint
{
  name      = waypt_return
  pwt       = 100
  condition = RETURN = true
  condition = DEPLOY = false
  perpetual = true
  updates   = RETURN_UPDATE
  endflag   = RETURN = false
  endflag   = DEPLOY = false
  endflag   = MISSION = complete
	

           speed = 2.0

  capture_radius = 2.0
     slip_radius = 8.0
          points = 0,0
 }
//-----------------------------------------------------
 Behavior = BHV_Loiter
 {
   // General Behavior Parameters
   // ---------------------------
   name         = loiter_west// example
   pwt          = 100                   // default
   //condition    = MODE==LOITERING       // example


   condition = DEPLOY = true
   condition = RETURN = false   
   condition = LOITER_REGION = west
   updates      = LOITER_UPDATE
   duration = 150
   endflag = LOITER_REGION = east
   duration_idle_decay = false
   perpetual  = true

   // Parameters specific to this behavior
   // ------------------------------------
        acquire_dist = 10               // default
      capture_radius = 3                // default  
     center_activate = false            // default
           clockwise = true             // default
         slip_radius = 15               // default
               speed = 2.5                // default
       spiral_factor = -2               // default
       

          // center_assign = 40,50            // example
    // xcenter_assign = 40               // example
     // ycenter_assign = 50               // example
    //上三行可覆盖polygon，形成新的目标中心
    
              polygon = radial:: x=100,y=-70,radius=30,pts=8 
         post_suffix = HENRY                             // example


        visual_hints = vertex_size  = 1             // default
        visual_hints = edge_size    = 1             // default
        visual_hints = vertex_color = dodger_blue   // default
        visual_hints = edge_color   = white         // default
        visual_hints = nextpt_color = yellow        // default
        visual_hints = nextpt_lcolor = aqua         // default
        visual_hints = nextpt_vertex_size = 5       // default
        visual_hints = label        = zone4         // example

 }
//----------------------------------------------
Behavior = BHV_Loiter
 {
   // General Behavior Parameters
   // ---------------------------
   name         = loiter_east// example
   pwt          = 100                   // default
   //condition    = MODE==LOITERING       // example


   condition = RETURN = false
   condition = DEPLOY = true
   condition = LOITER_REGION=east
   updates      = LOITER_UPDATE  
   duration = 150
   endflag = LOITER_REGION = west
     duration_idle_decay = false//<-------行为的持续时间时钟将保持暂停状态，直到满足其条件
   perpetual  = true


   // Parameters specific to this behavior
   // ------------------------------------
        acquire_dist = 10               // default
      capture_radius = 3                // default  
     center_activate = false            // default
           clockwise = false             // default
         slip_radius = 15               // default
               speed = 2.5                // default
       spiral_factor = -2               // default
       

          // center_assign = 40,50            // example
    // xcenter_assign = 40               // example
     // ycenter_assign = 50               // example
    //上三行可覆盖polygon，形成新的目标中心
    
         polygon = radial:: x=160,y=-50,radius=20,pts=8 
         post_suffix = HENRY                             // example


        visual_hints = vertex_size  = 1             // default
        visual_hints = edge_size    = 1             // default
        visual_hints = vertex_color = dodger_blue   // default
        visual_hints = edge_color   = white         // default
        visual_hints = nextpt_color = yellow        // default
        visual_hints = nextpt_lcolor = aqua         // default
        visual_hints = nextpt_vertex_size = 5       // default
        visual_hints = label        = zone3         // example

 }

//----------------------------------------------
//------------------------------------------------
Behavior=BHV_ConstantSpeed
{
  name      = const_speed
  pwt       = 200
  condition = SPD=true
  condition = DEPLOY = true
  perpetual = true
  updates   = SPEED_UPDATE
  endflag   = SPD = false

           speed = 0.5
    	    duration = 10
    duration_reset = CONST_SPD_RESET=true

}
```

#### 6.2.1任务4的变体

##### 目标：

用户在运行时通过鼠标单击选择第二个（东）徘徊位置？

##### 详解：

加一条命令就行

```
left_context[loiter_point] = LOITER_UPDATE = polygon = radial:: x=160,y=-50,radius=20,pts=8 
```

还有其他命令，只要能触发LOITER_REGION = east或着触发图形就可以，就是能明确的触发另一个悬停就可以

如下：

left_context[loiter_point] = LOITER_REGION = east 
 left_context[loiter_point] = LOITER_UPDATE = polygon = radial:: x=160,y=-50,radius=20,pts=8 
 left_context[loiter_point] = LOITER_UPDATE ="polygon = radial:: x=160,y=-50,radius=20,pts=8 "

这三条都可以，第三条的引号没有具体作用，有没有无所谓

##### 代码：

 就上面详解中的加一条代码就行，其他不变

### 6.3 任务 5 （勾选） - Bravo UUV 任务

#### 目标：

- 复制之前的 bravo 任务，在 missions/lab_05/ 文件夹中创建一个名为 bravo_loiter_uuv 的新目录。
- 如[第 7 节](https://oceanai.mit.edu/2.680/pmwiki/pmwiki.php?n=Lab.ClassHelmAutonomy#sec_depth_config)所述，对 [pMarinePIDV22](https://oceanai.mit.edu/ivpman/apps/pMarinePIDV22/index.html)、[uSimMarineV22](https://oceanai.mit.edu/ivpman/apps/uSimMarineV22/index.html)、[pHelmIvP](https://oceanai.mit.edu/ivpman/apps/pHelmIvP/index.html) 和 [pNodeReporter](https://oceanai.mit.edu/ivpman/apps/pNodeReporter/index.html) 进行更改。
- 将一对 [ConstantDepth](https://oceanai.mit.edu/ivpman/bhvs/ConstantDepth/index.html) 行为添加到行为文件中，并对其进行配置，以便车辆在西面徘徊时以 30 米的深度运行，在东面徘徊时以 10 米的深度运行

#### 详解：

**对要求拆解：**

​     对于 [pMarinePIDV22](https://oceanai.mit.edu/ivpman/apps/pMarinePIDV22/index.html)、[uSimMarineV22](https://oceanai.mit.edu/ivpman/apps/uSimMarineV22/index.html)、[pHelmIvP](https://oceanai.mit.edu/ivpman/apps/pHelmIvP/index.html) 和 [pNodeReporter](https://oceanai.mit.edu/ivpman/apps/pNodeReporter/index.html) 进行更改，在官方讲义的最后一页，复制粘贴就可以了 



（1）对于 pMarinePIDV22，它是moos-ivp的PID控制器

该代码的作用是**启用深度控制**，并设置**俯仰角 (Pitch) 和深度 (Z) 控制的 PID 参数**。

**1. `depth_control = true`**

- 这个配置表示**启用深度控制**，让 `pMarinePIDV22` 采用 Z 轴 PID 控制策略。
- 如果这个参数已经存在，它可能会被新的值覆盖。

**2. 俯仰角 (Pitch) PID 控制器**

```
   pitch_pid_kp                   = 1.5
   pitch_pid_kd                   = 0.3
   pitch_pid_ki                   = 0.004
   pitch_pid_integral_limit       = 0
```

- `pitch_pid_kp = 1.5`
  - **比例系数 (P)**，决定误差越大，控制力越大。
- `pitch_pid_kd = 0.3`
  - **微分系数 (D)**，用于抑制变化率过大的情况，减少震荡。
- `pitch_pid_ki = 0.004`
  - **积分系数 (I)**，用于消除稳态误差，但值太大会导致超调。
- `pitch_pid_integral_limit = 0`
  - **积分限制**，通常用于防止积分项积累过大导致失控 (Windup)，这里设置为 0，表示没有限制。

**3. 深度 (Z) 到俯仰角 (Pitch) PID 控制**

```
   z_to_pitch_pid_kp              = 0.12
   z_to_pitch_pid_kd              = 0.1
   z_to_pitch_pid_ki              = 0.004
   z_to_pitch_pid_integral_limit  = 0.05
```

- 这部分用于控制**深度 (Z) 通过调整俯仰角 (Pitch) 来改变**。
- `z_to_pitch_pid_kp = 0.12`
  - 控制水下航行器的深度误差如何转换为俯仰角调整。
- `z_to_pitch_pid_kd = 0.1`
  - 避免因深度误差快速变化而导致的剧烈调整。
- `z_to_pitch_pid_ki = 0.004`
  - 处理长期的深度误差，使航行器能够更精确地维持目标深度。
- `z_to_pitch_pid_integral_limit = 0.05`
  - 防止积分项积累过大，限制航行器的俯仰角变化幅度。

**4. 最大俯仰角 (Max Pitch) 和最大舵面角度 (Max Elevator)**

```
   maxpitch     = 15
   maxelevator  = 13
```

- `maxpitch = 15`

  - **最大俯仰角度数 (degrees)**，防止航行器过度俯仰导致不稳定。

- `maxelevator = 13`

  - **最大升降舵角度 (degrees)**，防止操纵舵面超出合理范围

    

（2）uSimMarineV22，他是MOOS-IvP 框架中的一个仿真模块，用于模拟水面和水下航行器的物理运动。
 要让 `uSimMarineV22` 模拟水下航行器

```
   buoyancy_rate        = 0.15
   max_depth_rate       = 5
   max_depth_rate_speed = 2.0
   default_water_depth  = 400
```

这些参数控制航行器的浮力和深度行为：

1. buoyancy_rate = 0.15
控制浮力调整速度，决定航行器的上浮/下潜速度。

值较小时，航行器浮力变化较慢，深度变化平稳。

值较大时，航行器会更快调整浮力，但可能导致剧烈震荡。

2. max_depth_rate = 5
最大深度变化速率，单位是 米/秒 (m/s)。

限制了航行器的上浮/下潜速度，确保其运动不会过快导致不稳定。

3. max_depth_rate_speed = 2.0
在 max_depth_rate (5m/s) 下允许的最大水平航行速度，单位是 节 (knots)。

确保航行器在快速上浮/下潜时，水平运动不会过快，以保证平稳控制。

4. default_water_depth = 400
默认水深 (米, m)，用于初始化时的默认起始深度。

适用于仿真环境，例如，默认让航行器在 400m 深度启动



(3) 修改pHelmIvp配置，pHelmIvP 是 MOOS-IvP 框架中的自主决策引擎，用于控制航行器的行为

扩展了舵手决策空间，以包括 101 个可能的深度决策。在更深的水域中，可以使用不同的配置。

```
  domain = depth:0:100:101
```

这个配置**扩展了 pHelmIvP 的决策空间**，允许它在**0 到 100 米深度范围内**做决策，并划分为 **101 个离散深度点**。

`depth` 代表深度是一个决策变量。

`0:100` 指定深度范围，从 **0 米 (水面)** 到 **100 米**。

`101` 代表**将深度划分为 101 个不同的离散值**，也就是说，每个深度增量约为 **1 米** (`(100-0) / (101-1) = 1` 米)。

（4）`pNodeReporter` 是 MOOS-IvP 框架中的一个进程，用于向外部可视化工具 (例如 `pMarineViewer`) 发送状态信息，包括航行器的位置、速度、深度等

（5）将一对 [ConstantDepth](https://oceanai.mit.edu/ivpman/bhvs/ConstantDepth/index.html) 行为添加到行为文件中，并对其进行配置，以便车辆在西面徘徊时以 30 米的深度运行，在东面徘徊时以 10 米的深度运行

[BHV_ConstantDepth](https://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=BHV.ConstantDepth)中提供了示例配置块，如下

```
 Behavior = BHV_ConstantDepth
 {
   // General Behavior Parameters
   // ---------------------------
   name         = const_dep_survey   // example
   pwt          = 100                // default
   condition    = MODE==SURVEYING    // example
   updates      = CONST_DEP_UPDATES  // example

   // Parameters specific to this behavior
   // ------------------------------------
            basewidth = 100          // default
                depth = 0            // default
   depth_mismatch_var = DEPTH_DIFF   // example
             duration = 0            // default (Choose something higher!)
            peakwidth = 3            // default
          summitdelta = 50           // default
 }
```

一个悬停，有一个BHV_ConstantDepth，所有要配置两个，不要忘了修改name来做区别，具体代码放到下面.bbhv中了

#### 代码：

```
///-------------------------------------------------
// FILE: bravo.moos
//-------------------------------------------------

ServerHost   = localhost
ServerPort   = 9000
Community    = bravo
MOOSTimeWarp = 10


// MIT Sailing Pavilion (use this one)
 LatOrigin  = 42.358456 
 LongOrigin = -71.087589

//------------------------------------------
// Antler configuration  block
ProcessConfig = ANTLER
{
  MSBetweenLaunches = 200

  Run = MOOSDB        @ NewConsole = false
  Run = pLogger       @ NewConsole = false
  Run = uSimMarineV22 @ NewConsole = false
  Run = pMarinePIDV22 @ NewConsole = false
  Run = pHelmIvP      @ NewConsole = false
  Run = pMarineViewer	@ NewConsole = false
  Run = uProcessWatch	@ NewConsole = false
  Run = pNodeReporter	@ NewConsole = false
  Run = pRealm	      @ NewConsole = false
}

//------------------------------------------
// pLogger config block

ProcessConfig = pLogger
{
  AppTick   = 8
  CommsTick = 8

  AsyncLog = true

  // For variables that are published in a bundle on their 
  // first post, explicitly declare their logging request
  Log = IVPHELM_LIFE_EVENT @ 0 NOSYNC
  Log = REPORT @ 0 NOSYNC
  Log = BHV_SETTINGS @ 0 NOSYNC
  Log = SUMMARY @ 0 NOSYNC

  LogAuxSrc = true
  WildCardLogging = true
  WildCardOmitPattern = *_STATUS
  WildCardOmitPattern = DB_VARSUMMARY
  WildCardOmitPattern = DB_RWSUMMARY
  WildCardExclusionLog = true
}

//------------------------------------------
// uProcessWatch config block

ProcessConfig = uProcessWatch
{
  AppTick   = 4
  CommsTick = 4

  watch_all = true
	nowatch   = uPokeDB*
	nowatch   = uQueryDB*
	nowatch   = uXMS*
	nowatch   = uMAC*
}

//------------------------------------------
// uSimMarineV22 config block

ProcessConfig = uSimMarineV22
{
  AppTick  	= 4
  CommsTick	= 4

  start_pos     = x=0, y=0, heading=180, speed=0
  prefix        = NAV
  turn_rate     = 95
  thrust_map    = 0:0, 20:1, 40:2, 60:3, 80:4, 100:5
  turn_spd_loss = 0.4

   buoyancy_rate        = 0.15
   max_depth_rate       = 5
   max_depth_rate_speed = 2.0
   default_water_depth  = 400
	
}

//------------------------------------------
// pHelmIvP config block

ProcessConfig = pHelmIvP
{
  AppTick    = 4
  CommsTick  = 4

  behaviors  = bravo.bhv
  domain     = course:0:359:360
  domain     = speed:0:4:41
  domain     = depth:0:100:101
}

//------------------------------------------
// pMarinePID config block

ProcessConfig = pMarinePIDV22
{
  AppTick    = 20
  CommsTick  = 20

  verbose       = true
  depth_control = true

  // SIM_INSTABILITY = 20

  // Yaw PID controller
  yaw_pid_kp		 = 0.9
  yaw_pid_kd		 = 0.3
  yaw_pid_ki		 = 0.3
  yaw_pid_integral_limit = 0.07

  // Speed PID controller
  speed_pid_kp		 = 1.0
  speed_pid_kd		 = 0.0
  speed_pid_ki		 = 0.0
  speed_pid_integral_limit = 0.07

  //Pitch PID controller
   pitch_pid_kp                   = 1.5
   pitch_pid_kd                   = 0.3
   pitch_pid_ki                   = 0.004
   pitch_pid_integral_limit       = 0

   //ZPID controller
   z_to_pitch_pid_kp              = 0.12
   z_to_pitch_pid_kd              = 0.1
   z_to_pitch_pid_ki              = 0.004
   z_to_pitch_pid_integral_limit  = 0.05

   maxpitch     = 15
   maxelevator  = 13

  //MAXIMUMS
  maxrudder    = 100
  maxthrust    = 100

  // A non-zero SPEED_FACTOR overrides use of SPEED_PID
  // Will set DESIRED_THRUST = DESIRED_SPEED * SPEED_FACTOR
  speed_factor = 20
}

//------------------------------------------
// pMarineViewer config block

ProcessConfig = pMarineViewer
{
  AppTick    = 4
  CommsTick  = 4

  tiff_file            = forrest19.tif
	//tiff_file            = MIT_SP.tif

  set_pan_x            = -90
  set_pan_y            = -280
  zoom                 = 0.65
  vehicle_shape_scale  = 1.5


  hash_delta           = 50
  hash_shade           = 0.22
  hash_viewable        = true

  trails_point_size   = 1

	//op_vertex = x=-83, y=-47,    lwidth=1,lcolor=yellow,looped=true,group=moa

  //op_vertex = x=-46.4, y=-129.2, lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=94.6, y=-62.2,   lwidth=1,lcolor=yellow,looped=true,group=moa
  //op_vertex = x=58,   y=20,    lwidth=1,lcolor=yellow,looped=true,group=moa

  // Appcast configuration
  appcast_height       = 75
  appcast_width        = 30
  appcast_viewable     = true
  appcast_color_scheme = indigo
  nodes_font_size      = xlarge
  procs_font_size      = xlarge
  appcast_font_size    = large

  // datum_viewable = true
  // datum_size     = 18
  // gui_size = small

 left_context[loiter_point] = DEPLOY=true
 left_context[loiter_point] = MOOS_MANUAL_OVERRIDE=false
 left_context[loiter_point] = RETURN=false


left_context[loiter_point] = LOITER_REGION = east 
 //left_context[loiter_point] = LOITER_UPDATE = polygon = radial:: x=160,y=-50,radius=20,pts=8 
 //left_context[loiter_point] = LOITER_UPDATE ="polygon = radial:: x=160,y=-50,radius=20,pts=8 "
 //上面这三条都可以

  right_context[return] = DEPLOY=true
  right_context[return] = MOOS_MANUAL_OVERRIDE=false
  right_context[return] = RETURN=false

  scope  = SUMMARY
  scope  = RETURN
  scope  = WPT_STAT
  scope  = VIEW_SEGLIST
  scope  = VIEW_POINT
  scope  = VIEW_POLYGON
  scope  = MVIEWER_LCLICK
  scope  = MVIEWER_RCLICK
  scope  = LOITER_REGION
  scope  = LOITER


  button_one = DEPLOY # DEPLOY=true # LOITER_REGION = west
  button_one = MOOS_MANUAL_OVERRIDE=false # RETURN=false
  button_two = RETURN # RETURN=true
  button_three = SLOWER # WPT_UPDATE=speed=1.5
  button_four = FASTER # WPT_UPDATE=speed=3.5

  action  = MENU_KEY=deploy # DEPLOY = true # RETURN = false
  action+ = MENU_KEY=deploy # MOOS_MANUAL_OVERRIDE=false
  action  = RETURN=true
  action  = UPDATES_RETURN=speed=1.4
}

//------------------------------------------
// pNodeReporter config block

ProcessConfig = pNodeReporter
{
  AppTick    = 2
  CommsTick	 = 2


  platform_type   = wamv
  platform_color  = yellow
  platform_length = 4

}
```

////////////////////////

```
///--------    FILE: bravo.bhv   -------------

initialize   DEPLOY = false
initialize   RETURN = false
	
//----------------------------------------------
Behavior = BHV_Waypoint
{ 
  name      = waypt_survey
  pwt       = 100
  condition = RETURN = false
  condition = DEPLOY = true
  //condition = LOITER_REGION = west
  endflag   = RETURN = true

	endflag   = SUMMARY=speed=$[SPEED],time=$[DUR_RUN_TIME]
	
	configflag = CRUISE_SPD = $[SPEED]
	
	activeflag = INFO=$[OWNSHIP]
	activeflag = INFO=$[BHVNAME]
	activeflag = INFO=$[BHVTYPE]
	
	cycleflag = CINFO=$[OSX],$[OSY]
	
	wptflag = PREV=$(PX),$(PY)
	wptflag = NEXT=$(NX),$(NY)
	wptflag = TEST=$(X),$(Y)
	wptflag = OSPOS=$(OSX),$(OSY)
	wptflag_on_start = true

  updates    = WPT_UPDATE
  perpetual  = true

	         speed_alt = 1.2
	   use_alt_speed = true
	            lead = 8
	     lead_damper = 1
	   lead_to_start = true
	           speed = 3   // meters per second
	    capture_line = true
	  capture_radius = 5.0
	     slip_radius = 15.0
	efficiency_measure = all
	
	           order = normal
	         repeat  = 0
	    
	visual_hints = nextpt_color=yellow
	visual_hints = nextpt_vertex_size=8
	visual_hints = nextpt_lcolor=gray70
	visual_hints = vertex_color=dodger_blue, edge_color=white
	visual_hints = vertex_size=5, edge_size=1

}

//----------------------------------------------
Behavior=BHV_Waypoint
{
  name      = waypt_return
  pwt       = 100
  condition = RETURN = true
  condition = DEPLOY = false
  perpetual = true
  updates   = RETURN_UPDATE
  endflag   = RETURN = false
  endflag   = DEPLOY = false
  endflag   = MISSION = complete
	

           speed = 2.0

  capture_radius = 2.0
     slip_radius = 8.0
          points = 0,0
 }
//-----------------------------------------------------
 Behavior = BHV_Loiter
 {
   // General Behavior Parameters
   // ---------------------------
   name         = loiter_west// example
   pwt          = 100                   // default
   //condition    = MODE==LOITERING       // example

   condition = DEPLOY = true
   condition = RETURN = false   
   condition = LOITER_REGION = west
   updates      = LOITER_UPDATE
   duration = 150
   endflag = LOITER_REGION = east
   duration_idle_decay = false
   perpetual  = true

   // Parameters specific to this behavior
   // ------------------------------------
        acquire_dist = 10               // default
      capture_radius = 3                // default  
     center_activate = false            // default
           clockwise = true             // default
         slip_radius = 15               // default
               speed = 2.5                // default
       spiral_factor = -2               // default
       

 polygon = radial:: x=100,y=-70,radius=30,pts=8 
 post_suffix = HENRY                             // example


        visual_hints = vertex_size  = 1             // default
        visual_hints = edge_size    = 1             // default
        visual_hints = vertex_color = dodger_blue   // default
        visual_hints = edge_color   = white         // default
        visual_hints = nextpt_color = yellow        // default
        visual_hints = nextpt_lcolor = aqua         // default
        visual_hints = nextpt_vertex_size = 5       // default
        visual_hints = label        = west         // example

 }
//----------------------------------------------
Behavior = BHV_Loiter
 {
   // General Behavior Parameters
   // ---------------------------
   name         = loiter_east// example
   pwt          = 100                   // default
   //condition    = MODE==LOITERING       // example

   condition = RETURN = false
   condition = DEPLOY = true
   condition = LOITER_REGION=east
   updates      = LOITER_UPDATE  
   duration = 150
   endflag = LOITER_REGION = west
     duration_idle_decay = false//<-------行为的持续时间时钟将保持暂停状态，直到满足其条件
   perpetual  = true


   // Parameters specific to this behavior
   // ------------------------------------
        acquire_dist = 10               // default
      capture_radius = 3                // default  
     center_activate = false            // default
           clockwise = false             // default
         slip_radius = 15               // default
               speed = 2.5                // default
       spiral_factor = -2               // default
       

          // center_assign = 40,50            // example
    // xcenter_assign = 40               // example
     // ycenter_assign = 50               // example
    //上三行可覆盖polygon，形成新的目标中心
    
         polygon = radial:: x=160,y=-50,radius=20,pts=8 
         post_suffix = HENRY                             // example


        visual_hints = vertex_size  = 1             // default
        visual_hints = edge_size    = 1             // default
        visual_hints = vertex_color = dodger_blue   // default
        visual_hints = edge_color   = white         // default
        visual_hints = nextpt_color = yellow        // default
        visual_hints = nextpt_lcolor = aqua         // default
        visual_hints = nextpt_vertex_size = 5       // default
        visual_hints = label        = east         // example

 }

//----------------------------------------------
Behavior = BHV_ConstantDepth
 {
   // General Behavior Parameters
   // ---------------------------
   name         = const_dep_survey_west   // example
   pwt          = 100                // default

   condition = DEPLOY = true
   condition = RETURN = false   
   condition = LOITER_REGION = west
   updates      = CONST_DEP_UPDATE  // example
   endflag = LOITER_REGION = east
   duration_idle_decay = false//<-------行为的持续时间时钟将保持暂停状态，直到满足其条件
   perpetual  = true

   // Parameters specific to this behavior
   // ------------------------------------
            basewidth = 100          // default
                depth = 30            // default
   depth_mismatch_var = DEPTH_DIFF   // example
             duration = 150            // default (Choose something higher!)
            peakwidth = 3            // default
          summitdelta = 50           // default
 }

 //----------------------------------------------
Behavior = BHV_ConstantDepth
 {
   // General Behavior Parameters
   // ---------------------------
   name         = const_dep_survey_east   // example
   pwt          = 100                // default

   condition = DEPLOY = true
   condition = RETURN = false   
   condition = LOITER_REGION = east
   updates      = CONST_DEP_UPDATE  // example
   endflag = LOITER_REGION = west
   duration_idle_decay = false//<-------行为的持续时间时钟将保持暂停状态，直到满足其条件
   perpetual  = true

   // Parameters specific to this behavior
   // ------------------------------------
            basewidth = 100          // default
                depth = 10           // default
   depth_mismatch_var = DEPTH_DIFF   // example
             duration = 150            // default (Choose something higher!)
            peakwidth = 3            // default
          summitdelta = 50           // default
 }
//------------------------------------------------
Behavior=BHV_ConstantSpeed
{
  name      = const_speed
  pwt       = 200
  condition = SPD=true
  condition = DEPLOY = true
  perpetual = true
  updates   = SPEED_UPDATE
  endflag   = SPD = false

           speed = 0.5
    	    duration = 10
    duration_reset = CONST_SPD_RESET=true

}
```

### 6.4 任务 6 （勾选） - Bravo UUV 表面任务

#### 目标：

- 复制之前的 bravo 任务，创建一个新的目录bravo_uuv_surface。
- 利用 helm Timer 行为来增强您的任务，让无人机定期（每 200 秒）浮出水面。在水面时，它应该停止，作为一个占位符，表面上是为了等待 GPS 定位。利用另一个在无人机到达水面时开始的 Timer 行为，等待 60 秒，然后再允许无人机再次俯冲

#### 详解：

 Behavior=BHV_Loiter_west实现第一个悬停， Behavior=BHV_Loiter_east实现第二个悬停，Behavior = BHV_ConstantDepth_west实现第一个悬停的30米水下，Behavior = BHV_ConstantDepth_east实现第二个悬停的10米水下

#### 代码：

这里，不要自定义行为Behavior = BHV_susu，没有BHV_susu.cpp/hpp做支撑,没法用，

就用官方提供的BHV_Timer和BHV_PeriodicSurface就行

**1. 全局初始化**

```moos
initialize   DEPLOY = false
initialize   RETURN = false
initialize   SURFACING = false
```

- 初始化三个状态变量（布尔值），默认均为false：
  - `DEPLOY`：是否部署任务
  - `RETURN`：是否返航
  - `SURFACING`：是否上浮

------

**2. 西部巡航行为（West Loiter）**

**(A) 巡航路径 (`BHV_Loiter`)**

```moos
Behavior=BHV_Loiter
{
  name=loiter_one
  clockwise=false                     // 逆时针巡航
  polygon = radial:: x=100,y=-75,radius=30,pts=8  // 中心点(100,-75)，半径30，8边形路径
  condition = DEPLOY=true             // 仅在 DEPLOY=true 时激活
  condition = LOITER_POS=west        // 巡航位置为西部
  condition = RETURN=false           // 非返航状态
  condition = SURFACING=false        // 非上浮状态
  perpetual=true                     // 持续运行
  endflag = LOITER_POS=east          // 结束时标记位置为东部
  duration = 150                     // 持续150秒
  duration_idle_decay = true        // 时间衰减,计时不停
  duration_reset = LOITER_POS=west   // 重置条件
  speed=2.5                          // 巡航速度2.5米/秒
}
```

**(B) 定深控制 (`BHV_ConstantDepth`)**

```moos
Behavior = BHV_ConstantDepth {
  name = loiter_depth_one
  condition = LOITER_POS=west        // 西部巡航时激活
  condition = DEPLOY=true
  depth = 30                         // 保持30米深度
  duration = 150                     
  duration_reset = LOITER_POS=west
  perpetual=true
}
```

------

**3. 东部巡航行为（East Loiter）**

**(A) 巡航路径 (`BHV_Loiter`)**

```moos
Behavior=BHV_Loiter
{
  name=loiter_two
  clockwise=false
  polygon = radial:: x=160,y=-50,radius=20,pts=8  // 中心点(160,-50)，半径20
  condition = DEPLOY=true
  condition = LOITER_POS=east       // 东部巡航
  condition = RETURN=false
  condition = SURFACING=false
  perpetual=true
  endflag = LOITER_POS=west         // 结束时标记位置为西部
  duration = 150
  duration_idle_decay = true
  duration_reset = LOITER_POS=east
  speed=2.5
}
```

**(B) 定深控制 (`BHV_ConstantDepth`)**

```moos
Behavior = BHV_ConstantDepth {
  name = loiter_depth_two
  condition = LOITER_POS=east
  condition = DEPLOY=true
  depth = 10                         // 东部巡航时深度较浅（10米）
  duration = 150
  duration_reset = LOITER_POS=east
  perpetual=true
}
```

------

**4. 返航行为 (`BHV_Waypoint`)**

```moos
Behavior=BHV_Waypoint
{
  name       = waypt_return
  pwt        = 100                   // 行为优先级权重
  condition  = RETURN = true         // 返航时激活
  condition  = DEPLOY = true
  perpetual  = true
  updates    = RETURN_UPDATE         // 动态更新路径点
  endflag    = RETURN = false        // 结束时重置状态
  endflag    = DEPLOY = false
  endflag    = MISSION = complete    // 标记任务完成
  speed = 2.0                        // 返航速度
  capture_radius = 2.0               // 抵达判定半径
  slip_radius = 8.0                  // 滑行半径
  points = 0,0                       // 返航目标点 (0,0)
}
```

------

**5. 恒定速度行为 (`BHV_ConstantSpeed`)**

```moos
Behavior=BHV_ConstantSpeed
{
  name       = const_speed
  pwt        = 200                   // 高优先级
  condition  = SPD=true              // SPD=true 时激活
  condition  = DEPLOY = true
  perpetual  = true
  updates    = SPEED_UPDATE          // 速度可动态更新
  endflag    = SPD = false           // 结束时重置
  speed = 0.5                        // 低速模式
  duration = 10                      // 持续10秒
  duration_reset = CONST_SPD_RESET=true
}
```

------

**6. 定期上浮行为**

**(A) 上浮到水面 (`BHV_ConstantDepth`)**

```moos
Behavior = BHV_ConstantDepth {
  name = periodic_surface
  condition = SURFACING=true        // 上浮时激活
  depth = 0                         // 深度0米（水面）
  perpetual=true
  depth_mismatch_var = DEPTH_DIFF   // 深度误差变量
}
```

**(B) 水面停留计时 (`BHV_Timer`)**

```moos
Behavior = BHV_Timer
{
  name = count_time_on_surface
  condition = DEPTH_DIFF=0          // 深度误差为0时开始计时
  condition = SURFACING=true
  perpetual=true
  duration = 60                     // 停留60秒
  duration_reset = SURFACING=false
  endflag = SURFACING=false         // 结束上浮
}
```

**(C) 上浮触发计时 (`BHV_Timer`)**

```moos
Behavior = BHV_Timer
{
  name = surface_timer
  condition = DEPLOY=true           // 任务部署后激活
  endflag = SURFACING=true          // 触发上浮
  perpetual = true
  duration = 200                    // 每200秒触发一次
  duration_reset = SURFACING=false
}
```

------

**行为逻辑总结**

1. 巡航模式
   - 西部巡航（深度30米）和东部巡航（深度10米）交替进行，通过 `LOITER_POS` 切换。
2. 返航
   - 当 `RETURN=true` 时，返回原点 `(0,0)` 并结束任务。
3. 定期上浮
   - 每200秒上浮一次，在水面停留60秒后继续任务。
4. 速度控制
   - 可通过 `SPD=true` 临时切换至低速模式（0.5米/秒）。

------

**关键变量**

|    变量名    |           作用            |
| :----------: | :-----------------------: |
|   `DEPLOY`   |         任务开关          |
| `LOITER_POS` | 巡航位置（`east`/`west`） |
|   `RETURN`   |         返航触发          |
| `SURFACING`  |         上浮触发          |
| `DEPTH_DIFF` |       深度误差监控        |

此配置适用于需要 **周期性巡航、深度控制、返航和上浮** 的水下机器人任务



### 6.5 任务 7 （checkoff） Bravo UUV Fair Time 任务



#### 目标：

在任务 6 中，当车辆浮出水面并在地面等待规定的时间时，当它最终恢复时，它通常会直接驶向对面的悬停区域。请再次查看[图 6.4](https://oceanai.mit.edu/2.680/pmwiki/pmwiki.php?n=Lab.ClassHelmAutonomy#fig_bravo_uuv_surface) 中的视频。为什么会这样？曲面序列开始时*处于活动状态*的 loiter 行为立即变为空闲状态，并且该 luiter 行为的持续时间时钟在曲面序列期间从未停止。

我们希望车辆在悬停器中断时剩余的持续时间内恢复其中断的悬停



#### 详解：

把loiter_one和loiter_two中的  duration_idle_decay = true改为  duration_idle_decay = false即可

#### 代码：

同6.4任务6



关于duration_status =  value (variable name)的使用

duration 参数指定行为超时并永久进入已完成状态 （） 之前的时间段（以秒为单位）。如果未指定，则行为没有时间限制。默认情况下，一旦舵处于驱动状态，持续时间时钟就会开始滴答作响。当行为随后进入空闲状态时，持续时间时钟将保持滴答作响。如果舵手暂时停放，它甚至会保持滴答作响。当发生超时时，将发布结束标志。可以使用 duration_status 参数将行为配置为发布超时前的剩余时间。每个表单的形式是：

```
    duration        =  value (positive numerical)
    duration_status =  value (variable name)
```

可以在在.bhv中设置全局变量initialize   REMAINING_TIME = 0

然后再loiter_one和loiter_two中添加 duration_status = REMAINING_TIME，且在.moos中添加scope

**关于上面添加变量，发布变量的用法，可以作为其他需要变量时的参考**

