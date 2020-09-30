@echo off

TITLE 棋牌数据库【Ver6.6_Spreader】 建立脚本启动中... [期间请勿关闭]

md D:\数据库

Rem 建主数据库二个
set rootPath=数据库脚本\
osql -E -i "%rootPath%数据库删除.sql"
osql -E -i "%rootPath%1_1_网站库脚本.sql"
osql -E -i "%rootPath%1_2_后台库脚本.sql"

osql -E -i "%rootPath%2_1_网站库脚本.sql"
osql -E -i "%rootPath%2_2_后台库脚本.sql"

Rem 建连接服务和填充数据
set rootPath=数据脚本\
osql -E -i "%rootPath%财富链接.sql"
osql -E -i "%rootPath%记录链接.sql"
osql -E -i "%rootPath%平台链接.sql"
osql -E -i "%rootPath%用户链接.sql"
osql -E -i "%rootPath%充值服务.sql"
osql -E -i "%rootPath%后台数据.sql"
osql -E -i "%rootPath%会员卡类型.sql"
osql -E -i "%rootPath%网站数据.sql"
osql -E -i "%rootPath%系统设置.sql"

Rem 存储过程
set rootPath=存储过程\前台脚本\金币数据库\
osql -E  -i "%rootPath%金币存款.sql"
osql -E  -i "%rootPath%金币取款.sql"
osql -E  -i "%rootPath%金币转账.sql"
osql -E  -i "%rootPath%实卡充值.sql"
osql -E  -i "%rootPath%推广中心.sql"
osql -E  -i "%rootPath%在线充值.sql"
osql -E  -i "%rootPath%在线订单.sql"
osql -E  -i "%rootPath%单个用户下所有被推荐人的推广信息.sql"

Rem 存储过程
set rootPath=存储过程\前台脚本\网站数据库\
osql -E  -i "%rootPath%比赛报名.sql"
osql -E  -i "%rootPath%问题反馈.sql"
osql -E  -i "%rootPath%更新浏览量.sql"
osql -E  -i "%rootPath%获取新闻信息.sql"

Rem 存储过程
set rootPath=存储过程\前台脚本\用户数据库\
osql -E  -i "%rootPath%固定机器.sql"
osql -E  -i "%rootPath%获取用户信息.sql"
osql -E  -i "%rootPath%奖牌兑换.sql"
osql -E  -i "%rootPath%魅力兑换.sql"
osql -E  -i "%rootPath%修改密码.sql"
osql -E  -i "%rootPath%修改资料.sql"
osql -E  -i "%rootPath%用户登录.sql"
osql -E  -i "%rootPath%用户名检测.sql"
osql -E  -i "%rootPath%用户全局信息.sql"
osql -E  -i "%rootPath%用户注册.sql"
osql -E  -i "%rootPath%账户保护.sql"
osql -E  -i "%rootPath%重置密码.sql"

Rem 存储过程
set rootPath=存储过程\前台脚本\积分数据库\
osql -E  -i "%rootPath%负分清零.sql"
osql -E  -i "%rootPath%逃率清零.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\函数\
osql -E  -i "%rootPath%网站_函数_切分字符串.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\记录库\
osql -E  -i "%rootPath%赠送会员.sql"
osql -E  -i "%rootPath%赠送金币.sql"
osql -E  -i "%rootPath%赠送经验.sql"
osql -E  -i "%rootPath%赠送靓号.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\平台库\
osql -E  -i "%rootPath%在线统计.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\金币库\
osql -E  -i "%rootPath%道具的增删改.sql"
osql -E  -i "%rootPath%实卡入库.sql"
osql -E  -i "%rootPath%实卡统计.sql"
osql -E  -i "%rootPath%网站_数据汇总统计.sql"
osql -E  -i "%rootPath%网站_赠送金币.sql"
osql -E  -i "%rootPath%新增卡生成记录.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\权限库\
osql -E  -i "%rootPath%用户表操作.sql"
osql -E  -i "%rootPath%网站_菜单加载.sql"
osql -E  -i "%rootPath%网站_管理员登录.sql"
osql -E  -i "%rootPath%网站_权限加载.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\积分库\
osql -E  -i "%rootPath%清零积分.sql"
osql -E  -i "%rootPath%清零逃率.sql"
osql -E  -i "%rootPath%赠送积分.sql"

Rem 存储过程
set rootPath=存储过程\后台脚本\帐号库\
osql -E  -i "%rootPath%网站_添加用户.sql"

Rem 存储过程
set rootPath=存储过程\公共过程\
osql -d QPAccountsDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPGameMatchDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPGameScoreDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPNativeWebDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPPlatformDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPPlatformManagerDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPRecordDB -E  -n -i "%rootPath%分页过程.sql"
osql -d QPTreasureDB -E  -n -i "%rootPath%分页过程.sql"

pause


COLOR 0A
CLS
@echo off
cls
echo ------------------------------
echo.
echo.		主要数据库建立完成，请根据自己平台的积分游戏执行 
echo.		存储过程\后台脚本\积分库  里的全部脚本
echo
echo ------------------------------

pause


