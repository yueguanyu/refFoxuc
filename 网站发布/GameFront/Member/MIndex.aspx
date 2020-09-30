<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="MIndex.aspx.cs" Inherits="Game.Web.Member.MIndex" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Info" Src="~/Themes/Standard/User_Info.ascx" %>
<%@ Register TagPrefix="qp" TagName="Logon" Src="~/Themes/Standard/User_Logon.ascx" %>
<%@ Register TagPrefix="qp" TagName="UserSidebar" Src="~/Themes/Standard/User_Sidebar.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id="Head1" runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/user_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/game_layout.css" />    
    
    <script src="../js/jquery-1.5.2.min.js" type="text/javascript"></script>
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
				<div class="userRigthTitle">首页 - 帐号服务中心 - 会员服务</div>
				<div class="userRigthBg">
					<div class="title f14 bold lan">基本信息</div>
				  	<table width="96%" border="0" align="center" cellpadding="0" cellspacing="0">
                      <tr>
                        <td colspan="4">尊敬的 <span class="hong bold"><asp:Label ID="lblAccounts" runat="server"></asp:Label></span> ,欢迎您来到用户个人中心。</td>
                      </tr>
                      <tr>
                        <td width="15%" align="right" bgcolor="#f7f9fd">昵 　 称：</td>
                        <td width="29%" bgcolor="#f7f9fd"><asp:Label ID="lblNickName" runat="server"></asp:Label></td>
                        <td width="19%" align="right" bgcolor="#f7f9fd">性　　别：</td>
                        <td width="37%" bgcolor="#f7f9fd">
                        
                        <asp:Label ID="lblGender" runat="server"></asp:Label>
                                                
                        </td>
                      </tr>
                      <tr>
                        <td align="right">会员等级：</td>
                        <td><asp:Label ID="lblMember" runat="server"></asp:Label></td>
                        <td align="right">经 验 值：</td>
                        <td><asp:Label ID="lblExperience" runat="server"></asp:Label></td>
                      </tr>
                      <tr>
                        <td align="right" bgcolor="#F7F9FD">个性签名：</td>
                        <td colspan="3" bgcolor="#F7F9FD"><asp:Label ID="lblUnderWrite" runat="server"></asp:Label></td>
                      </tr>
                    </table>
					<br /><br />
					<div class="title f14 bold lan">快捷通道</div>
						<div class="userTD">
							<div><a href="/Service/Index.aspx" class="help" hidefocus="true"></a></div>
							<div><a href="/Games/Index.aspx" class="game" hidefocus="true"></a></div>
							<div><a href="/Pay/PayIndex.aspx" class="user" hidefocus="true"></a></div>
							<div><a href="/Member/ApplyProtect.aspx" class="protect" hidefocus="true"></a></div>
							<div><a href="/Member/ModifyLogonPass.aspx" class="pass" hidefocus="true"></a></div>
							<div><a href="/Member/InsureTransfer.aspx" class="bank" hidefocus="true"></a></div>
							<div class="clear"></div>
						</div>
					
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
