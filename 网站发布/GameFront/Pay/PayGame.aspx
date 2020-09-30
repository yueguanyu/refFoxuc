<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayGame.aspx.cs" Inherits="Game.Web.Pay.PayGame" %>
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
		<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">您选择了<span class="hong">“第三方游戏卡”</span>充值方式</div>
		
		<form name="fmStep1" runat="server" id="fmStep1">

        <script type="text/javascript">
            $(document).ready(function(){    
                $("#rbPayGType1").attr("checked","checked");
                
                $("#txtPayAccounts").blur(function(){ checkAccounts(); });
                $("#txtPayReAccounts").blur(function(){ checkReAccounts(); });
                
                $("#btnPay").click(function (){
                    return checkInput();
                });
                
                $("#rbPayGType1").click(function(){
                    $("#rbPayGType1").parent().addClass("hand");
                    $("#rbPayGType2").parent().addClass("gray");
                    $("#rbPayGType3").parent().addClass("gray");
                    $("#rbPayGType4").parent().addClass("gray");
                    $("#rbPayGType5").parent().addClass("gray");
                    
                    $("#rbPayGType1").parent().removeClass("gray");
                    $("#rbPayGType2").parent().removeClass("hand");
                    $("#rbPayGType3").parent().removeClass("hand");
                    $("#rbPayGType4").parent().removeClass("hand");
                    $("#rbPayGType5").parent().removeClass("hand");
                });
                 $("#rbPayGType2").click(function(){
                    $("#rbPayGType1").parent().addClass("gray");
                    $("#rbPayGType2").parent().addClass("hand");
                    $("#rbPayGType3").parent().addClass("gray");
                    $("#rbPayGType4").parent().addClass("gray");
                    $("#rbPayGType5").parent().addClass("gray");
                    
                    $("#rbPayGType1").parent().removeClass("hand");
                    $("#rbPayGType2").parent().removeClass("gray");
                    $("#rbPayGType3").parent().removeClass("hand");
                    $("#rbPayGType4").parent().removeClass("hand");
                    $("#rbPayGType5").parent().removeClass("hand");
                });
                 $("#rbPayGType3").click(function(){
                    $("#rbPayGType1").parent().addClass("gray");
                    $("#rbPayGType2").parent().addClass("gray");
                    $("#rbPayGType3").parent().addClass("hand");
                    $("#rbPayGType4").parent().addClass("gray");
                    $("#rbPayGType5").parent().addClass("gray");
                    
                    $("#rbPayGType1").parent().removeClass("hand");
                    $("#rbPayGType2").parent().removeClass("hand");
                    $("#rbPayGType3").parent().removeClass("gray");
                    $("#rbPayGType4").parent().removeClass("hand");
                    $("#rbPayGType5").parent().removeClass("hand");
                });
                 $("#rbPayGType4").click(function(){
                    $("#rbPayGType1").parent().addClass("gray");
                    $("#rbPayGType2").parent().addClass("gray");
                    $("#rbPayGType3").parent().addClass("gray");
                    $("#rbPayGType4").parent().addClass("hand");
                    $("#rbPayGType5").parent().addClass("gray");
                    
                    $("#rbPayGType1").parent().removeClass("hand");
                    $("#rbPayGType2").parent().removeClass("hand");
                    $("#rbPayGType3").parent().removeClass("hand");
                    $("#rbPayGType4").parent().removeClass("gray");
                    $("#rbPayGType5").parent().removeClass("hand");
                });
                 $("#rbPayGType5").click(function(){
                    $("#rbPayGType1").parent().addClass("gray");
                    $("#rbPayGType2").parent().addClass("gray");
                    $("#rbPayGType3").parent().addClass("gray");
                    $("#rbPayGType4").parent().addClass("gray");
                    $("#rbPayGType5").parent().addClass("hand");
                    
                    $("#rbPayGType1").parent().removeClass("hand");
                    $("#rbPayGType2").parent().removeClass("hand");
                    $("#rbPayGType3").parent().removeClass("hand");
                    $("#rbPayGType4").parent().removeClass("hand");
                    $("#rbPayGType5").parent().removeClass("gray");
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
			  	<label class="hand"><input type="radio" id="rbPayGType1" name="rbPayGType" runat="server" value="C" checked="true" />盛大一卡通</label>
		        <label class="gray"><input type="radio" id="rbPayGType2" name="rbPayGType" runat="server" value="D" />征途游戏卡</label>
		        <label class="gray"><input type="radio" id="rbPayGType3" name="rbPayGType" runat="server" value="M" />网易一卡通</label>
		        <label class="gray"><input type="radio" id="rbPayGType4" name="rbPayGType" runat="server" value="N" />搜狐一卡通</label>
		        <label class="gray"><input type="radio" id="rbPayGType5" name="rbPayGType" runat="server" value="U" />完美一卡通</label>
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
				<label>提示：您选择的金额与游戏卡面额必须一致，否则将可能导致支付不成功、或支付余额丢失！</label>
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
			<p style="padding-bottom:8px;">3、支持全国通用的盛大一卡通，征途游戏卡，网易一卡通，搜狐一卡通，完美一卡通。 </p>
			<p style="padding-bottom:8px;">4、遇到任何充值问题，请您联系客服中心。</p>
		<div class="clear"></div>
		</div> 
		</form>
		
		<form id="fmStep2" runat="server" action="http://222.73.15.116/Pay_gatekq.aspx">
	     <div class="pay_tips">
            <p>
                <asp:Label ID="lblAlertIcon" runat="server"></asp:Label>
                <asp:Label ID="lblAlertInfo" runat="server" Text="操作提示"></asp:Label>
            </p>
            <p id="pnlContinue" runat="server">
                <input id="btnReset1" type="button" value="继续充值" onclick="goURL('/Pay/PayGame.aspx');" class="btn_home_4blue" />
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
