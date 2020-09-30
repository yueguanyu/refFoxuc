<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayVB.aspx.cs" Inherits="Game.Web.Pay.PayVB" %>
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
		<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">您选择了<span class="hong">“电话充值”</span>充值方式</div>
		
		<form name="fmStep1" runat="server" id="fmStep1">

    <script type="text/javascript">
        $(document).ready(function(){    
            $("#txtPayAccounts").blur(function(){ checkAccounts(); });
            $("#txtPayReAccounts").blur(function(){ checkReAccounts(); });
            
            $("#btnPay").click(function (){
                return checkInput();
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
			  	<span style="padding-top:5px;">充值金额：</span>	
			  	<input type="radio" id="rbSaleType1" name="rbSaleType" runat="server" value="10" checked="true" />使用10元充值卡，可以开通1个月，赠送50,000金币		  	
		 	</p>	
		 	<p style="text-align:left;padding-bottom:8px;">
				<span></span>
				<input type="radio" id="rbSaleType2" name="rbSaleType" runat="server" value="20" />使用20元充值卡，可以开通2个月，赠送100,000金币
	  	  	</p>			
			<p style="text-align:left;padding-bottom:8px;">
				<span></span>
				<input type="radio" id="rbSaleType3" name="rbSaleType" runat="server" value="30" />使用30元充值卡，可以开通3个月，赠送150,000金币
	  	  	</p>
			<p style="text-align:left;padding-bottom:8px;">
				<span></span>
			  	<input type="radio" id="rbSaleType4" name="rbSaleType" runat="server" value="50" />使用50元充值卡，可以开通5个月，赠送250,000金币
		  	</p>
			<p style="text-align:left;padding-bottom:13px;">
				<span></span>
			  	<input type="radio" id="rbSaleType5" name="rbSaleType" runat="server" value="100" />使用100元充值卡，可以开通10个月，赠送500,000金币
		  	</p>
		  	<p class="hong bold" style="text-align:left;">
		  		　提示：1.您选择的金额与获取的V币面额必须一致，否则将可能导致支付不成功、或支付余额丢失！<br />
				　　　　2.本站只支持10V币，20V币，30V币，50V币，100V币面额的V币！
			</p>	
        </div>        
		<div class="clear"></div>
		<div class="content_btn">
            <asp:Button ID="btnPay" runat="server" CssClass="btnL" Text="确 定" 
                onclick="btnPay_Click" />
        </div>  
		<div class="pay_tips_t" style="margin-top:15px;">
		    <span class="bold" style="text-align:left; padding-top:5px;">充值说明：</span>
		    <div><img src="/images/PayVB.gif"  alt="充值流程" style="border:solid 1px #b4b4b4;margin-bottom:10px;" /></div>
		    <iframe width="650" name="iframe" height="480"  frameborder="0" src="http://map.vnetone.com/default.aspx" scrolling="no"></iframe>
		</div>		
		
		</form>
		
		
		<asp:Panel ID="pnlStep2" runat="server">
            <div class="pay_tips">
                <p>
                    <asp:Label ID="lblAlertIcon" runat="server" Text=""></asp:Label> 
                    <asp:Label ID="lblAlertInfo" runat="server" Text="提示信息"></asp:Label>                    
                </p>
                <p id="pnlContinue" runat="server">
                    <input id="btnReset1" type="button" value="继续充值" onclick="goURL('/Pay/PayVB.aspx');" class="btn_home_4blue" />
                </p>
                <p id="pnlSubmit" runat="server" visible="false"> 
                    <form id="fmStep2" action="http://s2.vnetone.com/Default.aspx" method="post">
                        <asp:Literal ID="litVB" runat="server"></asp:Literal>
                        <script type="text/javascript">
                            window.onload = function() {                  
                                document.forms[0].submit(); 
                            }
                        </script>
	                </form>   
                </p>
            </div>            
        </asp:Panel>
		
	    
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
