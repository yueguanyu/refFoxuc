<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="InsureIn.aspx.cs" Inherits="Game.Web.Member.InsureIn" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Info" Src="~/Themes/Standard/User_Info.ascx" %>
<%@ Register TagPrefix="qp" TagName="Logon" Src="~/Themes/Standard/User_Logon.ascx" %>
<%@ Register TagPrefix="qp" TagName="UserSidebar" Src="~/Themes/Standard/User_Sidebar.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/user_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/game_layout.css" />   

    <script src="../js/jquery-1.5.2.min.js" type="text/javascript"></script>
    
    <script src="/JS/formValidator.js" type="text/javascript"></script>
    <script src="/JS/formValidatorRegex.js" type="text/javascript"></script>
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="userBody" style="margin-top:8px;">
	<div class="userTop"></div>
	<div class="userBg">
	    <!--左边页面开始-->
	    <div class="LeftSide">
			<% if (Fetch.GetUserCookie() == null)
           { %>
        <qp:Logon ID="sLogon" runat="server" />
        <%}
           else
           { %>
        <qp:Info ID="sInfo" runat="server" />
        <% } %>
			
<div class="LeftSide mtop10">

<qp:UserSidebar ID="sUserSidebar" runat="server" />

</div>		
<div class="clear"></div>					
		</div>
		<!--左边页面结束-->
		
		<!--右边页面开始-->		
		
		<div class="mainRight2">
			<div class="userRigthBody">
				<div class="userRigthTitle">会员中心 - 保险柜服务 - 存款</div>
				<div class="userRigthBg">
				<!--存款开始-->
				  <form name="form1" id="form1" runat="server">
				  <div class="title f14 bold lan">财富信息</div>
				  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
				    <tr>
				        <td width="20%"></td>	        
				        <td>保险柜存款：<asp:Label ID="lblInsureScore" runat="server" Text="0"></asp:Label> 金币  <a href="/Pay/PayIndex.aspx" class="ll">我要充值</a></td>
				    </tr>
				    <tr>
				        <td></td>	        
				        <td>现金余额：<asp:Label ID="lblScore" runat="server" Text="0"></asp:Label> 金币</td>
				    </tr>
				  </table>
				  <div class="title f14 bold lan">存入金币</div>
	              <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                    <tr>
                      <td width="20%" align="right">金币数目：</td>
                      <td width="80%"><asp:TextBox ID="txtScore" runat="server" CssClass="text" Text="0"></asp:TextBox></td>
                    </tr>
                    <tr>
                      <td align="right">备注信息：</td>
                      <td><asp:TextBox ID="txtNode" runat="server" CssClass="beizhu" TextMode="MultiLine"></asp:TextBox></td>
                    </tr>
                    <tr>
                      <td>&nbsp;</td>
                      <td>
                        <asp:Button ID="btnUpdate" Text="存入金币" runat="server" CssClass="bnt lan bold" 
                                onclick="btnUpdate_Click" />
                      </td>
                    </tr>
                  </table> 
                  <br />
                  <div class="title f14 bold lan">温馨提示</div>
                  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                    <tr>
                      <td width="10%" align="right"></td>
                      <td width="90%" class="hui6">
                        1、如果您在金币类游戏中，您将不能够存入金币。<br />
                        2、有任何问题，请您联系客服中心。<br />

                      </td>
                    </tr>
                  </table>          
                  </form>
	            <!--存款结束-->
                <div class="clear"></div>
		 	    </div>
				<div class="userRightBottom"><div class="clear"></div></div>
				<div class="clear"></div>
			</div>
			<div class="clear"></div>
		</div>
		<!--右边页面结束-->
		<div class="clear"></div>
	</div>
	<div class="userBottom"><div class="clear"></div></div>
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
