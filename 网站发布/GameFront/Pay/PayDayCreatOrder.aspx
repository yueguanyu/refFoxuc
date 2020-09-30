<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayDayCreatOrder.aspx.cs" Inherits="Game.Web.Pay.PayDayCreatOrder" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id="Head1" runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/pay_layout.css" />
    
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
        <div class="top">您的位置：首页&nbsp;<span class="arrow">»</span>&nbsp;充值中心&nbsp;<span class="arrow">»</span>&nbsp;帐号充值</div>
        <div class="center">
            <br />
            <div class="orderTitle">以下是您的订单信息</div>
            <form id="form1" action="http://pay.tiantianfu.com:8888/site/topup/pageSubmitInfo.php" runat="server" >
            <div class="orderBox">
	<span>充值帐号:</span>
	<label><b><%= userName%></b> </label>
	
	<span>充值金额:</span>
	<label><b class="hong"><%=money %>.00</b> RMB</label>
	
	<span>您的订单号:</span>
	<label><b><%=orderID %></b></label>
	
	<span>支付银行:</span>
	<label>
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="cmd" checked="checked" />招商银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="icbc" />工商银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="ccb" />建设银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="abc" />农业银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="boc" />中国银行</span>
		 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="pingan" />平安银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="cib" />兴业银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="cmbc" />民生银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="spdb" />浦发银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="comm" />交通银行</span> 
		
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="bos" />上海银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="citic" />中信银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="hxb" />华夏银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="hccb" />杭州银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="gzcb" />广州银行</span>
		 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="nbcb" />宁波银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="njcb" />南京银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="cbhb" />渤海银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="hkbchina" />汉口银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="qicbc" />工商企业银行</span> 
		
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="qccb" />建设企业银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="qabc" />农业企业银行</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="sde" />顺德农信社</span> 
		<span style="width:135px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="gdb" />广东发展银行</span> 
		
		<span style="width:160px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="gnxs" />广州市农村信用合作社</span>
		<span style="width:155px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="shrcb" />上海市农村商业银行</span> 
		<span style="width:140px; margin-left:0px; text-align:left;"><input name="bankCode" type="radio" value="ydxh" />尧都信用合作联社</span> 
	</label>
	<input type="hidden" value="<%= merId %>" id="merId" name="merId"/> 
	<input type="hidden" value="<%= orderID %>" id="orderId" name="orderId"/> 
	<input type="hidden" value="<%= money*100 %>" id="payMoney" name="payMoney"/> 
	<input type="hidden" value="<%= sendUrl %>" id="sendUrl" name="sendUrl"/> 
	<input type="hidden" value="<%= userName %>" id="userName" name="userName"/> 
	<input type="hidden" value="<%= notifyUrl %>" id="notifyUrl" name="notifyUrl"/> 
	<input type="hidden" value="<%= sign %>" id="sign" name="sign"/> 
	<input type="hidden" value="webnetbank" id="type" name="type"/>
<div class="clear"></div>
</div>
            <div style="margin-top:20px; text-align:center; margin-left:55px;width:300px;"><input name="Submit" type="submit" class="btnL" value="提交" /></div>
            </form>
        </div>
         <div class="bottom"></div> 
    </div>
    </div>
<!--页面主体结束-->
<qp:Footer ID="sFooter" runat="server" />
</body>
</html>
