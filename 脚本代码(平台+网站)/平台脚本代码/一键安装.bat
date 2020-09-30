@echo off

TITLE 棋牌数据库【Ver6.6_Spreader】 建立脚本启动中... [期间请勿关闭]

md D:\数据库

Rem 建主数据库三个
set rootPath=数据库脚本\
osql -E -i "%rootPath%数据库删除.sql"
osql -E -i "%rootPath%1_1_用户库脚本.sql"
osql -E -i "%rootPath%1_2_平台库脚本.sql"
osql -E -i "%rootPath%1_3_金币库脚本.sql"
osql -E -i "%rootPath%1_4_记录库脚本.sql"
osql -E -i "%rootPath%1_5_积分库脚本.sql"
osql -E -i "%rootPath%1_6_比赛库脚本.sql"
osql -E -i "%rootPath%1_7_练习库脚本.sql"

osql -E -i "%rootPath%2_1_用户库脚本.sql"
osql -E -i "%rootPath%2_2_平台库脚本.sql"
osql -E -i "%rootPath%2_3_金币库脚本.sql"
osql -E -i "%rootPath%2_4_记录库脚本.sql"
osql -E -i "%rootPath%2_5_积分库脚本.sql"
osql -E -i "%rootPath%2_6_比赛库脚本.sql"
osql -E -i "%rootPath%2_7_练习库脚本.sql"


Rem 建连接服务和填充数据
set rootPath=数据脚本\
osql -E -i "%rootPath%游戏列表.sql"
osql -E -i "%rootPath%财富链接.sql"
osql -E -i "%rootPath%道具配置.sql"
osql -E -i "%rootPath%记录链接.sql"
osql -E -i "%rootPath%平台链接.sql"
osql -E -i "%rootPath%系统配置.sql"
osql -E -i "%rootPath%用户链接.sql"

Rem 存储过程
set rootPath=存储过程\用户数据库\
osql -E  -i "%rootPath%绑定机器.sql"
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%修改密码.sql"
osql -E  -i "%rootPath%修改签名.sql"
osql -E  -i "%rootPath%用户资料.sql"
osql -E  -i "%rootPath%帐号登录.sql"
osql -E  -i "%rootPath%注册帐号.sql"
osql -E  -i "%rootPath%自定头像.sql"

set rootPath=存储过程\平台数据库\
osql -E  -i "%rootPath%房间管理.sql"
osql -E  -i "%rootPath%加载节点.sql"
osql -E  -i "%rootPath%加载类型.sql"
osql -E  -i "%rootPath%加载页面.sql"
osql -E  -i "%rootPath%加载种类.sql"
osql -E  -i "%rootPath%连接信息.sql"
osql -E  -i "%rootPath%模块管理.sql"
osql -E  -i "%rootPath%在线信息.sql"
osql -E  -i "%rootPath%加载敏感词.sql"

set rootPath=存储过程\金币数据库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%道具管理.sql"
osql -E  -i "%rootPath%道具配置.sql"
osql -E  -i "%rootPath%加载机器.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%加载消息.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%列表配置.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%查询用户.sql"
osql -E  -i "%rootPath%银行服务.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏写分.sql"
osql -E  -i "%rootPath%银行查询用户.sql"

set rootPath=存储过程\积分数据库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%道具管理.sql"
osql -E  -i "%rootPath%道具配置.sql"
osql -E  -i "%rootPath%加载机器.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%加载消息.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%列表配置.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏写分.sql"

set rootPath=存储过程\比赛数据库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%道具管理.sql"
osql -E  -i "%rootPath%加载机器.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%加载消息.sql"
osql -E  -i "%rootPath%开始结束.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%列表配置.sql"
osql -E  -i "%rootPath%设置权限.sql"
osql -E  -i "%rootPath%写入费用.sql"
osql -E  -i "%rootPath%写入奖励.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏写分.sql"

set rootPath=存储过程\练习数据库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%道具管理.sql"
osql -E  -i "%rootPath%道具配置.sql"
osql -E  -i "%rootPath%加载机器.sql"
osql -E  -i "%rootPath%加载配置.sql"
osql -E  -i "%rootPath%加载消息.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%列表描述.sql"
osql -E  -i "%rootPath%列表配置.sql"
osql -E  -i "%rootPath%游戏记录.sql"
osql -E  -i "%rootPath%游戏写分.sql"


pause

CLS
@echo off
@echo 正在生成游戏ID ……
set rootPath=数据脚本\
osql -E  -i "%rootPath%标识生成.sql"

COLOR 0A
CLS
@echo off
cls
echo ------------------------------
echo.
echo	主要数据库建立完成，请根据自己平台的积分游戏执行 
echo.
echo.
echo	版权所有： 深圳市网狐科技有限公司
echo ------------------------------

pause


