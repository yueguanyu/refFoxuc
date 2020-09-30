<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="USPay.aspx.cs" Inherits="Game.Web.UserService.USPay" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/pay_layout.css"  />
</head>
<body style="background-image:none;width:330px;">
 <div id="content" style="float:left; min-height:100px; background-image:none; width:330px">
        <!--右边内容头部-->
        <!--右边内容中部-->
        <div class="center" style="min-height:0px; width:330px;">
        <div class="payRechargeTitle" style="text-align:left; padding-top:5px; width:330px;">推荐充值方式</div>
        <div class="payRechargeCard" style="margin-left:20px;">
            <span><a href="/Pay/PayOnLine.aspx" class="card1" hideFocus="ture" target="_blank"></a></span>
            <label>网银大额充值，安全、快捷的在线充值服务，即充即可到帐，1分钟完成！</label>
            <a href="/Pay/PayOnLine.aspx" class="btnRecharge" hideFocus="ture" target="_blank"></a>
            <div class="clear"></div>
        </div>
        <div class="payRechargeCard" style="margin-left:20px;">
            <span><a href="/Pay/PayCardFill.aspx" class="card2" hideFocus="ture" target="_blank"></a></span>
            <label>游戏家园发行的充值卡，安全、快捷，购买后便可充值！</label>
            <a href="/Pay/PayCardFill.aspx" class="btnRecharge" hideFocus="ture" target="_blank"></a>
            <div class="clear"></div>
        </div>
        <div class="more" style="text-align:right; padding-top:5px;width:330px;"><a href="/Pay/PayIndex.aspx" target="_blank" style="color:Black;">更多充值方式</a></div>
		</div>
        <div class="clear"></div>
</div>
</body>
</html>
