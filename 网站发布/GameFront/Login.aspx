<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Login.aspx.cs" Inherits="Game.Web.Login" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    <link rel="stylesheet" type="text/css" href="style/layout.css" />
    <link rel="stylesheet" type="text/css" href="style/global.css" />

    <script src="js/jquery-1.5.2.min.js" type="text/javascript"></script>
   
    <script type="text/javascript">
        function UpdateImage(){
            document.getElementById("picVerifyCode").src="../../ValidateImage.aspx?r="+Math.random();
        }
        
        $(document).ready(function(){     
            $("#txtAccounts").focus();
        });
        
        function checkInput(){    
            if($("#txtAccounts").val()==""){
                alert("请输入您的用户名！");
                $("#txtAccounts").focus();
                return false;
            }
            if($("#txtLogonPass").val()==""){
                alert("请输入您的密码！");
                $("#txtLogonPass").focus();
                return false;
            }
            if($("#txtCode").val()==""){
                alert("请输入您的验证码！");
                $("#txtCode").focus();
                return false;
            }
            return true;
        }
    </script>
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main">
    <form name="form1" runat="server">
	<div class="logonBody">
		<div class="left"><img src="images/login01.jpg" border="0" usemap="#Map" />
        <map name="Map" id="Map"><area shape="rect" coords="6,154,142,178" href="Register.aspx" /></map>
        </div>
		<div class="left"><img src="images/login02.jpg" /></div>
		<div class="left"><img src="images/login03.jpg" /></div>
 	    <div class="logonBg">
			<div class="left"><img src="images/login04.jpg" /></div>
	      <div class="logonBg1">
	        <table width="277" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="70" height="35" class="f14" align="right">帐　号：</td>
                  <td width="207" height="35"><asp:TextBox ID="txtAccounts" runat="server" CssClass="text"></asp:TextBox></td>
                </tr>
                <tr>
                  <td class="f14" align="right">密　码：</td>
                  <td><asp:TextBox ID="txtLogonPass" TextMode="Password" runat="server" CssClass="text" Width="149"></asp:TextBox></td>
                </tr>
                <tr>
                  <td class="f14" align="right">验证码：</td>
                  <td><asp:TextBox ID="txtCode" runat="server" CssClass="text"></asp:TextBox></td>
                </tr>
                <tr>
                  <td></td>
                  <td class="hui6">验证码请按下图中的数字填写</td>
                </tr>
                <tr>
                  <td></td>
                  <td><img src="ValidateImage.aspx" name="picVerifyCode" id="ImageCheck" onclick="UpdateImage()" style="cursor:pointer;border: 1px solid #333333;" title="点击更换验证码图片!" /></td>
                </tr>
                <tr>
                  <td></td>
                  <td><a href="javascript:void(0)" onclick="UpdateImage()" class="ll" id="ImageCheck2">看不清楚？ 换一个</a></td>
                </tr>
                <tr>
                  <td></td>
                  <td><asp:ImageButton ID="btnLogon" OnClientClick="return checkInput()" 
                          runat="server" ImageUrl="images/login1.gif" Width="86" Height="33" 
                          onclick="btnLogon_Click" />&nbsp;&nbsp;<a href="Register.aspx"><img src="images/reg1.gif" width="86" height="33" border="0" /></a></td>
                </tr>
              </table>
	      </div>
			<div class="logonBg2"><label><a href="/Member/ReLogonPass.aspx" class="lh">忘记密码？</a></label></div>
			<div class="left"><img src="images/login06.jpg" /></div>
			<div class="clear"></div>
	    </div>
		<div class="left"><img src="images/login07.jpg" /></div>
		<div class="clear"></div>
	</div>
	
	</form>
	<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
