<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayCardFill.aspx.cs" Inherits="Game.Web.Pay.PayCardFill" %>
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
    <script src="/js/inputCheck.js" type="text/javascript"></script>
    <script src="/js/utils.js" type="text/javascript"></script>
    
    <script type="text/javascript">
        $(document).ready(function(){
            //页面验证
             $("#txtAccounts").blur(function(){
                checkAccounts();
            });
            
            $("#txtAccounts2").blur(function(){ checkAccounts2(); });
            $("#txtSerialID").blur(function(){ checkSerial(); });
            $("#txtPassword").blur(function(){ checkPassword(); });
            
            $("#btnPay").click(function(){
                return checkInput();
            });
        });
        
        function checkInput(){
            if(!checkAccounts()){
                $("#txtAccounts").focus();return false; 
            }
            
            if(!checkAccounts2()){$("#txtAccounts2").focus(); return false; }
            if(!checkSerial()){$("#txtSerialID").focus(); return false; }
            if(!checkPassword()){$("#txtPassword").focus(); return false; }
        }
        
        function checkAccounts(){
            if($.trim($("#txtAccounts").val())==""){
                $("#txtAccountsTip").html("请输入您的游戏账号");                
                return false;
            }
            var reg = /^[a-zA-Z0-9_\u4e00-\u9fa5]+$/;
            if(!reg.test($("#txtAccounts").val())){
                $("#txtAccountsTip").html("游戏账号是由字母、数字、下划线和汉字的组合！");                
                return false;
            }
            
            if(byteLength($("#txtAccounts").val())<4){
                $("#txtAccountsTip").html("游戏账号的长度至少为4个字符");                
                return false;
            }
            if($("#txtAccounts").val().length>31){
                $("#txtAccountsTip").html("游戏账号的长度不能超过31个字符");          
                return false;
            }
            $("#txtAccountsTip").html("");    
            return true;
        }
        
        function checkAccounts2(){
            if($.trim($("#txtAccounts2").val())==""){
                $("#txtAccounts2Tip").html("请输入您的游戏账号");
                return false;
            }
            if($("#txtAccounts2").val()!=$("#txtAccounts").val()){
                $("#txtAccounts2Tip").html("两次输入的游戏账号不一致");
                return false;
            }
            $("#txtAccounts2Tip").html("");
            return true;
        }
        
        function checkSerial(){
            if($.trim($("#txtSerialID").val())==""){    
                $("#txtSerialIDTip").html("请输入您的充值卡号");
                return false;
            }
            $("#txtSerialIDTip").html("");
            return true;
        }
        
        function checkPassword(){
            if($.trim($("#txtPassword").val())==""){
                $("#txtPasswordTip").html("请输入您的充值密码");
                return false;
            }
            $("#txtPasswordTip").html("");
            return true;
        }
    </script>
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
        <div class="top">您的位置：首页&nbsp;<span class="arrow">»</span>&nbsp;充值中心&nbsp;<span class="arrow">»</span>&nbsp;实卡充值</div>
	    <!--右边内容中部-->
        <div class="center">
		<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">实卡充值</div>
		
		
		<form name="fmStep1" runat="server" id="fmStep1">

		<div class="pay_content">
			<div class="content">
				<p style="padding-bottom:8px;">
				    <span style="padding-top:5px;">游戏帐号：</span>
				    <label><asp:TextBox ID="txtAccounts" runat="server" CssClass="width206 text" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
				    <label id="txtAccountsTip" style="padding-top:5px; color:Red;"></label>
				</p>
				<p style="padding-bottom:8px;">
				    <span style="padding-top:5px;">确定帐号：</span>
				    <label><asp:TextBox ID="txtAccounts2" runat="server" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
				    <label id="txtAccounts2Tip" style="padding-top:5px; color:Red;"></label>
				</p>
				<p style="padding-bottom:8px;">
				    <span style="padding-top:5px;">充值卡号：</span>
				    <label><asp:TextBox ID="txtSerialID" runat="server" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
				    <label id="txtSerialIDTip" style="padding-top:5px; color:Red;"></label>
				</p>
				<p style="padding-bottom:8px;">
				    <span style="padding-top:5px;">充值密码：</span>
				    <label><asp:TextBox ID="txtPassword" runat="server" TextMode="Password" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
				    <label id="txtPasswordTip" style="padding-top:5px; color:Red;"></label>
			    </p>
			</div>
		
		    <div class="clear"></div>
		    <div class="content_btn">
                <asp:Button ID="btnPay" runat="server" Text="充 值" CssClass="btnL" OnClick="btnPay_Click" />
			    <input name="btnReset" type="reset" class="btnR" value="取 消" />
		    </div>
		</div>

		</form>
		
		
        <form id="fmStep2" runat="server">
	     <div class="pay_tips">
            <p>
                <asp:Label ID="lblAlertIcon" runat="server"></asp:Label>
                <asp:Label ID="lblAlertInfo" runat="server" Text="操作提示"></asp:Label>
                <asp:Literal ID="lit99Bill" runat="server"></asp:Literal>
            </p>
            <p id="pnlContinue" runat="server">
                <input id="btnReset1" type="button" value="继续充值" onclick="goURL('/Pay/PayCardFill.aspx');" class="btn_home_4blue" />
            </p>
        </div>        
    </form>
		
		<div class="height10"></div>
		<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">购卡网点</div>
		
		<div class="pay_table">
            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#a9ccdd">
            <tr>
                <td width="10%" class="bgcolor_easyblue text_c">编号</td>
                <td width="50%" class="bgcolor_easyblue text_c">网点</td>
                <td width="20%" class="bgcolor_easyblue text_c">电话</td>
                <td width="20%" class="bgcolor_easyblue text_c">备注</td>
            </tr>
            <tr style="height:30px;">
                <td class="bgcolor_white text_c">1</td>
                <td class="bgcolor_white text_c">广东省深圳市福田区香梅北青海大厦712-709室</td>
                <td class="bgcolor_white text_c">0755-83547947</td>
                <td class="bgcolor_white text_c">&nbsp;</td>
            </tr>            
            </table>
   	  	</div>
		
		<div class="height10"></div>
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
