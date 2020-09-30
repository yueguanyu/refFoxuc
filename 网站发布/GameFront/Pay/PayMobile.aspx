<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayMobile.aspx.cs" Inherits="Game.Web.Pay.PayMobile" %>
<%@ Import Namespace="Game.Facade" %>

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
		<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">您选择了<span class="hong">“手机充值卡”</span>充值方式</div>
		
		<form name="fmStep1" runat="server" id="fmStep1">

        <script type="text/javascript">
            $(document).ready(function(){    
                $("#txtPayAccounts").blur(function(){ checkAccounts(); });
                $("#txtPayReAccounts").blur(function(){ checkReAccounts(); });
                
                $("#btnPay").click(function (){
                    return checkInput();
                });
                
                $("#rbPayMType1").click(function(){
                    $("#lblPayMType2").addClass("mlink1");
                    $("#lblPayMType3").addClass("mlink1");
                    $("#lblPayMType1").addClass("mlink");
                    
                    $("#lblPayMType2").removeClass("mlink");
                    $("#lblPayMType3").removeClass("mlink");
                    $("#lblPayMType1").removeClass("mlink1");
                });
                $("#rbPayMType2").click(function(){
                    $("#lblPayMType1").addClass("mlink1");
                    $("#lblPayMType3").addClass("mlink1");
                    $("#lblPayMType2").addClass("mlink");
                    
                    $("#lblPayMType1").removeClass("mlink");
                    $("#lblPayMType3").removeClass("mlink");
                    $("#lblPayMType2").removeClass("mlink1");
                });
                $("#rbPayMType3").click(function(){
                    $("#lblPayMType1").addClass("mlink1");
                    $("#lblPayMType2").addClass("mlink1");
                    $("#lblPayMType3").addClass("mlink");
                    
                    $("#lblPayMType1").removeClass("mlink");
                    $("#lblPayMType2").removeClass("mlink");
                    $("#lblPayMType3").removeClass("mlink1");
                });
            });
            
            function checkAccounts(){
                if($.trim($("#txtPayAccounts").val())==""){
                    $("#txtPayAccountsTips").html("请输入您的充值账号");
                    return false;
                }
                $("#txtPayAccountsTips").html("");
                return true;
            }
            
            function checkReAccounts(){
                if($.trim($("#txtPayReAccounts").val())==""){
                    $("#txtPayReAccountsTips").html("请再次输入充值账号");
                    return false;
                }
                if($("#txtPayReAccounts").val()!=$("#txtPayAccounts").val()){
                    $("#txtPayReAccountsTips").html("两次输入的账号不一致");
                    return false;
                }
                $("#txtPayReAccountsTips").html("");
                return true;
            }
            
            function checkInput(){
                if(!checkAccounts()){$("#txtPayAccounts").focus(); return false; }
                if(!checkReAccounts()){$("#txtPayReAccounts").focus(); return false; }
            }
        </script>

		<div class="content">
		    <div class="height10"></div>
            <p style="padding-bottom:8px;">
                <span style="padding-top:5px;">充值用户名：</span>
                <label><asp:TextBox ID="txtPayAccounts" runat="server" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
                <label style=" padding-top:5px; color:Red" id="txtPayAccountsTips"></label>
            </p>
            <p style="padding-bottom:8px;">
                <span style="padding-top:5px;">确认用户名：</span>
                <label><asp:TextBox ID="txtPayReAccounts" runat="server" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
                <label style=" padding-top:5px; color:Red;" id="txtPayReAccountsTips"></label>
            </p>							
		    <p style="text-align:left;padding-bottom:8px;">
			  	<span style="padding-top:5px;">充值卡类别：</span>
			  	<label><input type="radio" id="rbPayMType1" runat="server" name="rbPayMType" value="1" checked="true" /></label>
			    <label id="lblPayMType1" class="mlink" for="rbPayMType1" hidefocus="true">神州行充值卡</label>
			    <label><input type="radio" id="rbPayMType2" runat="server" name="rbPayMType" value="2" /> </label>
			    <label id="lblPayMType2" class="mlink1" for="rbPayMType2" hidefocus="true">联通充值卡</label>
			    <label><input type="radio" id="rbPayMType3" runat="server" name="rbPayMType" value="3" /> </label>
			    <label id="lblPayMType3" class="mlink1" for="rbPayMType3" hidefocus="true">电信充值卡</label>
		 	</p>				
			<p style="text-align:left;padding-bottom:8px;">
				<span></span>
				<input type="radio" id="rbSaleType1" name="rbSaleType" value="30" runat="server" checked="true" />使用30元充值卡，可以开通3个月，赠送300,000金币
	  	  	</p>
			<p style="text-align:left;padding-bottom:8px;">
				<span></span>
			  	<input type="radio" id="rbSaleType2" name="rbSaleType" value="50" runat="server" />使用50元充值卡，可以开通5个月，赠送500,000金币
		  	</p>
			<p style="text-align:left;padding-bottom:13px;">
				<span></span>
			  	<input type="radio" id="rbSaleType3" name="rbSaleType" value="100" runat="server" />使用100元充值卡，可以开通10个月，赠送1,000,000金币
		  	</p>
		  	<p class="hong bold" style="text-align:left;">
		  		<span></span>
				<label>提示：1．您选择的金额与充值卡面额必须一致，否则将可能导致支付不成功、或支付余额丢失！</label>
				<label>　　　2．联通卡，电信卡只支持50，100面额</label>
			</p>	
        </div>
        
		<div class="clear"></div>
		<div class="content_btn">
            <asp:Button ID="btnPay" runat="server" CssClass="btnL" Text="确 定" 
                onclick="btnPay_Click" />
        </div>  
        <div class="pay_tips_t" style="text-align:left;">
			<span class="bold" style="text-align:left; padding-top:5px;">温馨提示：</span>
			<p style="padding-bottom:8px;">1、请确保您填写的用户名正确无误，因玩家输入错误而导致的任何损失由用户自行承担。</p>
			<p style="padding-bottom:8px;">2、充值过程中，浏览器会跳转至银行页面，支付成功后，会自动返回家园网站，如果没有跳转或是弹出充值成功的页面，请您不要关闭充值窗口。</p>
			<p style="padding-bottom:8px;">3、支持全国通用的神州行卡（卡号17位，密码18位）。面额： 10  ,30  ,50  ,100  ,200  ,300  ,500  元。 </p>
			<p style="padding-bottom:8px;">4、支持江苏移动充值卡（卡号16位，密码17位）。面额： 30  ,50  ,100  元。</p>
			<p style="padding-bottom:8px;">5、支持浙江移动充值卡（卡号10位，密码8位）。面额： 10  ,30  ,50  ,100  元。</p>
			<p style="color:Red; padding-bottom:8px;">6、支持全国通用的联通卡（卡号15位，密码19位）。面额： 50  ,100  元。 </p>
			<p style="color:Red; padding-bottom:8px;">7、支持全国通用的电信卡（卡号19位，密码18位）。面额： 50  ,100  元。</p>
			<p>8、遇到任何充值问题，请您联系客服中心。</p>
		<div class="clear"></div>
		</div> 
		</form>
		
		<form id="fmStep2" runat="server" action="http://www.99bill.com/szxgateway/recvMerchantInfoAction.htm">
	     <div class="pay_tips">
            <p>
                <asp:Label ID="lblAlertIcon" runat="server"></asp:Label>
                <asp:Label ID="lblAlertInfo" runat="server" Text="操作提示"></asp:Label>
                <asp:Literal ID="lit99Bill" runat="server"></asp:Literal>
            </p>
            <p id="pnlContinue" runat="server">
                <input id="btnReset1" type="button" value="继续充值" onclick="goURL('/Pay/PayMobile.aspx');" class="btn_home_4blue" />
            </p>
        </div>        
    </form>
	    
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
