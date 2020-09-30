<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Sitemap.aspx.cs" Inherits="Game.Web.Sitemap" %>
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
			<div class="mapTitle"></div>
		  <div class="tgBg">
				<div class="tgTitle1 bold">网站导航</div>
           		<div class="content">
					<ul>
					<li><a href="Index.aspx" class="lw">首页</a></li>
					<li><a href="/News/NewsList.aspx" class="lw">新闻公告</a></li>
					<li><a href="/Member/MIndex.aspx" class="lw">会员中心</a></li>
					<li><a href="/Pay/PayIndex.aspx" class="lw">充值中心</a></li>
					<li><a href="/Match/Index.aspx" class="lw">比赛中心</a></li>
					<li><a href="/Shop/ShopIndex.aspx" class="lw">游戏商场</a></li>
					<li><a href="/Spread/SpreadIndex.aspx" class="lw">推广系统</a></li>
					<li><a href="/Service/Index.aspx" class="lw">客服中心</a></li>
					</ul>
				<div class="clear"></div>
				</div>
				
				<div class="tgTitle1 bold">会员中心</div>
				<div class="content">
					<ul>
					<li><a href="/Member/ModifyNikeName.aspx" class="lw">修改昵称</a></li>
					<li><a href="/Member/ModifyUserInfo.aspx" class="lw">修改资料</a></li>
					<li><a href="/Member/ModifyFace.aspx" class="lw">修改头像</a></li>
					<li><a href="/Member/ModifyLogonPass.aspx" class="lw">修改登录密码</a></li>
					<li><a href="/Member/ModifyInsurePass.aspx" class="lw">修改保险柜密码</a></li>
					<li><a href="/Member/ApplyProtect.aspx" class="lw">申请密码保护</a></li>
					<li><a href="/Member/ModifyProtect.aspx" class="lw">修改密码保护</a></li>
					<li><a href="/Member/ReLogonPass.aspx" class="lw">找回登录密码</a></li>
					<li><a href="/Member/ReInsurePass.aspx" class="lw">找回保险柜密码</a></li>
					<li><a href="/Member/ApplyPasswordCard.aspx" class="lw">申请密保卡</a></li>
					<li><a href="/Member/ExitPasswordCard.aspx" class="lw">取消密保卡</a></li>
					<li><a href="/Member/InsureIn.aspx" class="lw">存款</a></li>
					<li><a href="/Member/InsureOut.aspx" class="lw">取款</a></li>
					<li><a href="/Member/InsureTransfer.aspx" class="lw">转帐</a></li>
					<li><a href="/Member/InsureRecord.aspx" class="lw">交易明细</a></li>
					<li><a href="/Member/ConvertPresent.aspx" class="lw">魅力兑换</a></li>
					<li><a href="/Member/ConvertRecord.aspx" class="lw">魅力兑换记录</a></li>
					<li><a href="/Member/ConvertMedal.aspx" class="lw">奖牌兑换</a></li>
					<li><a href="/Member/ConvertMedalRecord.aspx" class="lw">奖牌兑换记录</a></li>
					<li><a href="/Member/ClearScore.aspx" class="lw">负分清零</a></li>
					<li><a href="/Member/ClearFlee.aspx" class="lw">逃跑清零</a></li>
					<li><a href="/Member/GameRecord.aspx" class="lw">游戏记录</a></li>
					<li><a href="/Member/MoorMachine.aspx" class="lw">固定机器</a></li>
					<li><a href="/Member/SpreadIn.aspx" class="lw">业绩查询</a></li>
					<li><a href="/Member/SpreadOut.aspx" class="lw">结算查询</a></li>
					<li><a href="/Member/SpreadBalance.aspx" class="lw">业绩结算</a></li>
					<li><a href="/Member/SpreadInfo.aspx" class="lw">推广明细</a></li>
					</ul>
				<div class="clear"></div>
				</div>
				
				<div class="tgTitle1 bold">充值中心</div>
   		    	<div class="content">
					<ul>
					<li><a href="/Pay/PayCardFill.aspx" class="lw">实卡充值</a></li>
					<li><a href="/Pay/PayOnline.aspx" class="lw">网银充值</a></li>
					<li><a href="/Pay/PayMobile.aspx" class="lw">手机充值卡</a></li>
					<li><a href="/Pay/PayGame.aspx" class="lw">第三方游戏卡</a></li>
					<li><a href="/Pay/PayVB.aspx" class="lw">电话充值</a></li>
					</ul>
				<div  class="clear"></div>
				</div>
				
				<div class="tgTitle1 bold">客服中心</div>
   		    	<div class="content">
					<ul>
					<li><a href="/Service/Index.aspx" class="lw">新手帮助</a></li>
					<li><a href="/Service/IssueList.aspx" class="lw">常见问题</a></li>
					<li><a href="/Service/FeedbackList.aspx" class="lw">问题反馈</a></li>
					<li><a href="/Service/Customer.aspx" class="lw">客服电话</a></li>
					</ul>
				<div  class="clear"></div>
				</div>
				
				<div class="tgTitle1 bold">下载中心</div>
   		    	<div class="content">
					<ul>
					<li><a href="http://download.foxuc.com/loader/whgame/unicode/WHGamePlaza.exe" class="lw">精简版下载</a></li>
					<li><a href="http://download.foxuc.com/loader/whgame/unicode/WHGameFull.exe" class="lw">完整版下载</a></li>
					</ul>
				<div  class="clear"></div>
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
