<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="MatchApply.aspx.cs" Inherits="Game.Web.Match.MatchApply" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/contest_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/news_layout.css" />
    
    <script src="../js/jquery-1.5.2.min.js" type="text/javascript"></script>
    
    <script src="/JS/formValidator.js" type="text/javascript"></script>
    <script src="/JS/formValidatorRegex.js" type="text/javascript"></script>
    
    <script type="text/javascript">
        $(document).ready(function() {
            //页面验证
            $.formValidator.initConfig({ formid: "form1", alertmessage: true, onerror: function(msg) { alert(msg) } });
            $("#txtAccounts").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "游戏帐号不能为空！" })
            $("#txtPwd").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "游戏密码不能为空！" })
            $("#txtCompellation").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "真实姓名不能为空！" })
            $("#txtPassportID").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "身份证号不能为空！" }).functionValidator({ fun: isCardID });
            $("#txtMobilePhone").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "联系电话不能为空！" })
            $("#txtEMail").formValidator().inputValidator({ min: 6, max: 100, onerror: "你输入的邮箱长度非法,请确认" }).regexValidator({ regexp: "^([\\w-.]+)@(([[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.)|(([\\w-]+.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(]?)$", onerror: "你输入的邮箱格式不正确" });
            $("#txtQQ").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "QQ不能为空！" }).regexValidator({ regexp: "^[1-9][0-9]*$", onerror: "你输入的QQ号码格式不正确" })
            $("#txtDwellingPlace").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "详细地址不能为空！" })
            $("#txtPostalCode").formValidator().inputValidator({ min: 1, empty: { leftempty: false, rightempty: false, emptyerror: "两边不能有空符号！" }, onerror: "邮编不能为空！" }).regexValidator({ regexp: "^[0-9]{6}$", onerror: "你输入的邮政编码格式不正确" })
        })
    </script>
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main" style="margin-top:8px;">
	<div class="contTop">
		<div class="nav">
			<div class="left"></div>
			<div class="center">
				<ul>
				<li><a href="/Match/MatchView.aspx?XID=<%= matchID %>">赛事公告</a></li>
				<li><a href="/Match/MatchOrder.aspx?XID=<%= matchID %>">比赛排名</a></li>
				<li><a href="/Match/MatchQuery.aspx?XID=<%= matchID %>">个人成绩</a></li>
				</ul>
		  	</div>
			<div class="right"></div>
		</div>
	</div>
	
	
    <div class="contMain">
	<div class="top1"></div>
    <div class="mainBox">
	    <div class="mainBox1 f12 lineHeight25">
	    
	        <div id="divRight" runat="server"></div>
	        
	        <form id="form1" name="form1" runat="server">
	   		<table width="59%" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td height="35" align="right">游戏帐号： </td>
                <td height="35"><asp:TextBox ID="txtAccounts" runat="server" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>
              <tr>
                <td height="35" align="right">游戏密码： </td>
                <td height="35"><asp:TextBox ID="txtPwd" TextMode="Password" runat="server" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>
              <tr>
                <td height="35" align="right">真实姓名： </td>
                <td height="35"><asp:TextBox ID="txtCompellation" runat="server" CssClass="text width200" MaxLength="5"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>
              <tr>
                <td height="35" align="right">性别： </td>
                <td height="35">
                    <input type="radio" name="radGender" id="rdoMale" runat="server" value="1" checked="true" />男                    
                    <input type="radio" name="radGender" id="rdoFemale" runat="server" value="2"/>女                    
                </td>
              </tr>              
              <tr>
                <td height="35" align="right">身份证号：</td>
                <td height="35"><asp:TextBox ID="txtPassportID" runat="server" CssClass="text width200" MaxLength="18"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>              
              <tr>
                <td height="35" align="right">联系电话：</td>
                <td height="35"><asp:TextBox ID="txtMobilePhone" runat="server" CssClass="text width200" MaxLength="16"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>
              <tr>
                <td height="35" align="right">电子邮件：</td>
                <td height="35"><asp:TextBox ID="txtEMail" runat="server" CssClass="text width200" MaxLength="30"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>   
              <tr>
                <td height="35" align="right">QQ：</td>
                <td height="35"><asp:TextBox ID="txtQQ" runat="server" CssClass="text width200" MaxLength="16"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>           
              <tr>
                <td height="35" align="right">详细地址：</td>
                <td height="35"><asp:TextBox ID="txtDwellingPlace" runat="server" CssClass="text width200" MaxLength="200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>
              <tr>
                <td height="35" align="right">邮编：</td>
                <td height="35"><asp:TextBox ID="txtPostalCode" runat="server" CssClass="text width200" MaxLength="6"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
              </tr>              
              <tr>
                <td height="15"></td>
                <td height="15"></td>
              </tr>
              <tr>
                <td height="35">&nbsp;</td>
                <td height="35">
                    <asp:Button ID="btnConfirm" runat="server" CssClass="btn1" Text="提 交" 
                        onclick="btnConfirm_Click" /> &nbsp;&nbsp;
                    <input type="reset" name="Submit2" value="重 置" class="btn1" />
                </td>
              </tr>
            </table>
            </form>
  	    </div>  	    
  	</div>
	<div class="bottom"></div>
    </div>
    
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
