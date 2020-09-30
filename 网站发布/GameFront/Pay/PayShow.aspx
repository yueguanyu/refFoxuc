<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayShow.aspx.cs" Inherits="Game.Web.Pay.PayShow" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>
<head id="Head1" runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/pay_layout.css"  />
    
    <script src="/js/jquery-1.5.2.min.js" type="text/javascript"></script>
    <script src="/js/utils.js" type="text/javascript"></script>
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
	
	<!--右边开始-->
    <div id="content">
        <!--右边内容头部-->
        <div class="top">您的位置：首页&nbsp;<span class="arrow">»</span>&nbsp;充值中心&nbsp;<span class="arrow">»</span>&nbsp;帐号充值</div>
	    <!--右边内容中部-->
        <div class="center">
        <div class="step2"></div>
		<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">请确认您的订单信息</div>
		
		<div class="content">
		    <div class="textBox">
				<div class="box">
					<div class="title" style="text-align:left;"><asp:Literal ID="lblResults" runat="server"></asp:Literal></div>
					<div class="info">
						<ul>
						    <li><span>订 单 号：</span><asp:Label ID="lblOrderID" runat="server"></asp:Label></li>
						    <li><span>家园帐号：</span><asp:Label ID="lblAccounts" runat="server"></asp:Label></li>
						    <li><span>帐 号 ID：</span><asp:Label ID="lblGameID" runat="server"></asp:Label></li>
						    <li><span>购卡类别：</span><asp:Label ID="lblCardName" runat="server"></asp:Label></li>
						    <li><span>会员月数：</span><asp:Label ID="lblMemberMonth" runat="server"></asp:Label></li>
						    <li><span>赠送金币：</span><asp:Label ID="lblPresentScore" runat="server"></asp:Label></li>
						    <li><span>订单金额：</span><asp:Label ID="lblOrderAmount" runat="server"></asp:Label></li>
 						</ul>
					</div>
					<div class="clear"></div>
				</div>
			</div>
		    <div class="height10"></div>   		   
        </div>	
		<div class="clear"></div>       
	    
		<div class="height10"></div>           
	    <div class="clear"></div>
	    </div>  	    
    <div class="bottom"></div> 
    </div>
    
    </div>
   
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
