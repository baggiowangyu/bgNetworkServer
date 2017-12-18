========================================================================
    控制台应用程序：bgHttpServer 项目概述
========================================================================

- 接口路径：/goldmsg/car/DeviceControl
------------------------------------------------------------------------
- 云台命令(返回值无实际操控意义)：
  - 方向控制：
	{
		"id":"",
		"commandtype":"PTZControl",
		"subcmd":"direction",
		"value":"Up|UpRight|Right|DownRight|Down|DownLeft|Left|UpLeft",
		"speed":"1-8"
	}
  - 镜头控制：
	{
		"id":"",
		"commandtype":"PTZControl",
		"subcmd":"camera",
		"value":"Plus|Minus",
	}
  - 焦距控制：
	{
		"id":"",
		"commandtype":"PTZControl",
		"subcmd":"focus",
		"value":"Plus|Minus",
	}
  - 光圈控制：
	{
		"id":"",
		"commandtype":"PTZControl",
		"subcmd":"aperture",
		"value":"Plus|Minus",
	}
- 点流控制命令：
  - 视频点流（返回流URL）
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"commandtype":"stream",
		"subcmd":"realvideo",
		"value":"",
	}
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"commandtype":"stream",
		"subcmd":"realvideo",
		"value":"rtsp://123.123.123.123/car/yueA-12345/8",
	}