<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="SpreadIndex.aspx.cs" Inherits="Game.Web.Spread.SpreadIndex" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/news_layout.css" />
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main" style="margin-top:8px;">
	<!--左边部分开始-->
	<!--左边部分开始-->
<div class="mainLeft1">
	<div class="LeftSide">		
		<qp:Btn ID="sBtn" runat="server" />
		
		<qp:Question ID="sQuestion" runat="server" />
		
		<qp:Service ID="sService" runat="server" />
		
	<div class="clear"></div>
	</div>
	<div class="clear"></div>
</div>
<!--左边部分结束-->		
	<!--左边部分结束-->
	
	<!--右边部分开始-->
	
	<div class="mainRight1">
		<!--新闻公告开始-->
		<div class="newsBody">
		<div class="tgTitle"></div>
		  <div class="tgBg">
				<div class="tgTitle1 lan f14 bold">1、游戏推广员制度</div>
           		<div class="content">　　经推荐的玩家游戏时间达到30个小时，推荐人将获得200000金币的一次性奖励。
把推荐人填为您的游戏帐号，您将立即获得5000金币
</div>
				
				<div class="tgTitle1 lan f14 bold">2、推荐方式</div>
<div class="content"><span class="bold">方式一</span>：<br />
           		  （1）介绍新用户进http://qp6603.foxuc.net/ 下载游戏并安装。<br />
           		  <span class="bold">方式二</span>：<br />
           		  （1）在公共上网场所（如网吧），进http://qp6603.foxuc.net/下载游戏并安装。<br />
           		  （2）在安装游戏的目录下建立文件custom\introduce.txt，文件内容设置为：[config] introducer = 您的游戏帐号<br />
           		  在网吧上网的用户。
                  <br />
          举例：您把面对面视频游戏安装在D:\Program Files\ttmdm下，你的游戏帐号为aaaa 。 那么就先在ttmdm目录下建立custom
          <br />
          </div>
				
				<div class="tgTitle1 lan f14 bold">3、推广业绩</div>
<div class="content">（1）被推荐玩家游戏时间达到30个小时，您将获得200000金币的一次性奖励；
  <br />
  游戏时间：是指在各个游戏中,进行游戏的总时间,不包含挂机时间。游戏时间一旦达到30个小时，该玩家则成为有效用户。
<br />
           		  （2）被推荐玩家充值,每充值1元钱,您将获得被推荐玩家充值金币10%；<br />
           		  （3）推荐玩家推广业绩的10%。当您所推荐的玩家，也推荐了其他人时,他（她）每次结算推广业绩时（将推广获得的金币转入游戏帐户中）,系统同时赠送您10%的金额，他的提成收入保持不变。这样,您间接推荐的玩家都有可能为您带来业绩。<br />
	        <span class="bold">例如：您推荐了玩家A，A又推荐了好多其他的玩家。当A结算业绩时，获得了2000万金币提成，您的业绩同时也增加200万金币</span></div>
				
				<div class="tgTitle1 lan f14 bold">4、推广员业绩查询</div>
       		    <div class="content">
       		    推广员业绩查询和结算请登录会员中心
       		    </div>		
		  </div>
		  <div class="clear"></div>
		  <div class="newsBottom"><div class="clear"></div></div>
		<div class="clear"></div>
		</div>
		<!--新闻公告结束-->
	<div class="clear"></div>
 	</div>
	<!--右边部分结束-->
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
