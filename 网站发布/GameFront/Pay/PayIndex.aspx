<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayIndex.aspx.cs" Inherits="Game.Web.Pay.PayIndex" %>
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
    <link rel="stylesheet" type="text/css" href="/style/pay_layout.css"  />
    
    <script src="/js/jquery-1.5.2.min.js" type="text/javascript"></script>
</head>
<body>

<qp:Header ID="sHeader" runat="server" />


<script type="text/javascript">
    $(document).ready(function(){    
        $("#cbType").attr("checked","");
    });
</script>

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
        <div class="step1"></div>
        <div class="payRechargeTitle" style="text-align:left; padding-top:5px;">推荐充值方式</div>

        <div class="payRechargeCard">
            <span><a href="/Pay/PayOnLine.aspx" class="card1" hideFocus="ture"></a></span>
            <label>网银大额充值，安全、快捷的在线充值服务，即充即可到帐，1分钟完成！</label>
            <a href="/Pay/PayOnLine.aspx" class="btnRecharge" hideFocus="ture"></a>
            <div class="clear"></div>
        </div>

        <div class="payRechargeCard">
            <span><a href="/Pay/PayCardFill.aspx" class="card2" hideFocus="ture"></a></span>
            <label>游戏家园发行的充值卡，安全、快捷，购买后便可充值！</label>
            <a href="/Pay/PayCardFill.aspx" class="btnRecharge" hideFocus="ture"></a>
            <div class="clear"></div>
        </div>
        
        <div class="payRechargeCard">
            <span><a href="/Pay/PayYB.aspx" class="card12" hideFocus="ture"></a></span>
            <label>网银大额充值，安全、快捷的在线充值服务，即充即可到帐，1分钟完成</label>
            <a href="/Pay/PayYB.aspx" class="btnRecharge" hideFocus="ture"></a>
            <div class="clear"></div>
        </div>
        
        <div class="payRechargeCard">
            <span><a href="/Pay/PayMobile.aspx" class="card6" hideFocus="ture"></a></span>
            <label>支持全国通用的神州行手机充值卡（卡号17位，密码18位）。</label>
            <a href="/Pay/PayMobile.aspx" class="btnRecharge" hideFocus="ture"></a>
            <div class="clear"></div>
        </div>

        <div class="payRechargeTitle" style="text-align:left; padding-top:5px;">其他充值方式</div>       

        <div class="payRechargeCard">
            <span><a href="/Pay/PayMobile.aspx" class="card5" hideFocus="ture"></a></span>
            <label>支持全国通用的联通手机充值卡（卡号15位，密码19位）。</label>
            <a href="/Pay/PayMobile.aspx" class="btnRecharge" hideFocus="ture"></a>
            <div class="clear"></div>
        </div>
        
        <div class="payRechargeCard">
			<span><a href="/Pay/PayMobile.aspx" class="card7" hideFocus="ture"></a></span>
			<label>支持全国通用的电信手机充值卡（卡号19位，密码18位）。</label>
			<a href="/Pay/PayMobile.aspx" class="btnRecharge" hideFocus="ture"></a>
			<div class="clear"></div>
		</div>
        
        <div class="payRechargeCard">
            <span><a href="/Pay/PayVB.aspx" class="card4" hideFocus="ture"></a></span>
            <label>全国电话用户（含部分手机）通过当地电信业务获取V币，再凭V币到家园网站充值。</label>
            <a href="/Pay/PayVB.aspx" class="btnRecharge" hideFocus="ture"></a>
            <div class="clear"></div>
        </div>  
        
        <div class="payRechargeCard">
			<span><a href="/Pay/PayGame.aspx" class="card8" hideFocus="ture"></a></span>
			<label>支持全国通用的盛大一卡通 （卡号 15 位，密码 8 位）。 </label>
			<a href="/Pay/PayGame.aspx" class="btnRecharge" hideFocus="ture"></a>
			<div class="clear"></div>
		</div>      

        <div class="payRechargeCard">
			<span><a href="/Pay/PayGame.aspx" class="card3" hideFocus="ture"></a></span>
			<label>支持全国通用的网易一卡通 （卡号 13 位，密码 9 位）。 </label>
			<a href="/Pay/PayGame.aspx" class="btnRecharge" hideFocus="ture"></a>
			<div class="clear"></div>
		</div>	
		
		<div class="payRechargeCard">
		    <span><a href="/Pay/PayGame.aspx" class="card9" hideFocus="ture"></a></span>
		    <label>支持全国通用的征途游戏卡 （卡号 16 位，密码 8 位）。 </label>
		    <a href="/Pay/PayGame.aspx" class="btnRecharge" hideFocus="ture"></a>
		    <div class="clear"></div>
	    </div>
		
		<div id="moreType" style="display:none">	    
    		
		    <div class="payRechargeCard">
			    <span><a href="/Pay/PayGame.aspx" class="card10" hideFocus="ture"></a></span>
			    <label>支持全国通用的搜狐一卡通 （卡号 20 位，密码 12 位）。</label>
			    <a href="/Pay/PayGame.aspx" class="btnRecharge" hideFocus="ture"></a>
			    <div class="clear"></div>
		    </div>
    		
		    <div class="payRechargeCard">
			    <span><a href="/Pay/PayGame.aspx" class="card11" hideFocus="ture"></a></span>
			    <label>支持全国通用的完美一卡通 （卡号 10 位，密码 15 位）。</label>
			    <a href="/Pay/PayGame.aspx" class="btnRecharge" hideFocus="ture"></a>
			    <div class="clear"></div>
		    </div>
		</div>
		<div class="more" style="text-align:left; padding-top:5px;"><input id="cbType" name="" type="checkbox" value="" onclick="if(document.getElementById('moreType').style.display=='none'){document.getElementById('moreType').style.display='block';}else{document.getElementById('moreType').style.display='none'}" /><label for="cbType" >更多充值方式</label></div>

        <div class="clear"></div>
        </div>
        <div class="bottom"></div>  
    </div>
    
    </div>
    
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
