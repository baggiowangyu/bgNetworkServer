========================================================================
    控制台应用程序：bgHttpServer 项目概述
========================================================================

- 接口路径：/goldmsg/car/DeviceControl
------------------------------------------------------------------------
- 云台命令(云台控制指令是一个指令开关，当发送某个操作的开指令之后，设备会持续执行这个指令，直到发送关指令为止)：
  - 方向控制：
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"manufacturer":"dh|hik|gxx",
		"commandtype":"PTZControl",
		"subcmd":"direction",
		"value":"Up|UpRight|Right|DownRight|Down|DownLeft|Left|UpLeft",
		"state":"Start|Stop",
		"speed":1-7
	}
  - 镜头控制：
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"manufacturer":"dh|hik|gxx",
		"commandtype":"PTZControl",
		"subcmd":"camera",
		"value":"Plus|Minus",
		"state":"Start|Stop",
		"speed":1-7
	}
  - 焦距控制：
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"manufacturer":"dh|hik|gxx",
		"commandtype":"PTZControl",
		"subcmd":"focus",
		"value":"Plus|Minus",
		"state":"Start|Stop",
		"speed":1-7
	}
  - 光圈控制：
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"manufacturer":"dh|hik|gxx",
		"commandtype":"PTZControl",
		"subcmd":"aperture",
		"value":"Plus|Minus",
		"state":"Start|Stop",
		"speed":1-7
	}
- 点流控制命令：
  - 视频点流（返回流URL）
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"manufacturer":"dh|hik|gxx",
		"commandtype":"stream",
		"subcmd":"realvideo",
		"value":""
		"state":"Start|Stop"
	}
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"commandtype":"stream",
		"subcmd":"realvideo",
		"value":"rtsp://123.123.123.123/car/yueA-12345/8",
	}
  - 查询实时流地址
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"manufacturer":"dh|hik|gxx",
		"commandtype":"stream",
		"subcmd":"query_realvideo_url",
	}
	{
		"id":"56C90602-ECA8-4350-9208-BE9141743E61",
		"commandtype":"stream",
		"subcmd":"query_realvideo_url",
		"stream_url":"rtsp://123.123.123.123/car/yueA-12345/8"
	}

- 直播流地址：/goldmsg/car/Live.flv?manufacturer=dh
- 测试直播流地址：http://127.0.0.1:9876/goldmsg/car/Live.flv?manufacturer=dh