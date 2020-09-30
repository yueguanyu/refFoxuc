<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ClearFlee.aspx.cs" Inherits="Game.Web.Member.ClearFlee" %>
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
				<div class="userRigthTitle">会员中心 - 会员服务 - 逃跑清零</div>
				    <div class="userRigthBg">
				    <!--逃跑清零开始-->
				    <form id="form1" runat="server" runat="server">
				    <div class="userRigthBg1">
                      <span><asp:Label ID="lblAccounts" runat="server"></asp:Label>，你有以下<asp:Label ID="lblNumber" runat="server"></asp:Label>款游戏需要进行逃跑清零：</span>
                      <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class="leftTop">
                        <tr align="center" class="bold">
                          <td class="leftTop trBg">游戏名称</td>
                          <td class="leftTop trBg">逃跑数</td>
                          <td class="leftTop trBg">清零</td>
                        </tr>
                          <asp:Repeater ID="rptFleeList" runat="server" 
                                    onitemcommand="rptFleeList_ItemCommand">
                            <ItemTemplate>
                                <tr align="center" class="trBg1">
                                  <td><%# Eval("KindName") %></td>
                                  <td><%# Eval("FleeCount") %></td>
                                  <td>
                                    <asp:LinkButton ID="btnClear" runat="server" CommandName="Clear" CommandArgument='<%# Eval("KindID") %>' OnClientClick="return confirm('确定要清零吗?')" Text="清零"></asp:LinkButton>
                                  </td>
                                </tr>
                            </ItemTemplate>
                          </asp:Repeater>
                      </table>
                      <div class="clear"></div>
                    </div>
	                <!--逃跑清零结束-->
	                </form>
	                
	                <div class="loginHint" id="divRight" runat="server">
	                    
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
