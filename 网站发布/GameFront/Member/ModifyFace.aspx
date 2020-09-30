<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ModifyFace.aspx.cs" Inherits="Game.Web.Member.ModifyFace" %>
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

    <script src="/js/jquery-1.5.2.min.js" type="text/javascript"></script>
    
    <link type="text/css" href="/js/lhgdialog/lhgdialog.css" rel="stylesheet"/>
    <script src="/JS/lhgdialog/lhgcore.min.js" type="text/javascript"></script>
    <script src="/JS/lhgdialog/lhgdialog.js" type="text/javascript"></script>
    <script type="text/javascript">
	    $(document).ready(function(){
            //弹出页面
            J('#btnSwitchFace').dialog({ id: 'winUserfaceList', title: '更换头像', width:540,height:385, page: '/FaceList.aspx',rang: true,cover: true });
        })		        
	</script> 
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
				<div class="userRigthTitle">会员中心 - 帐号服务 - 修改头像</div>
				<div class="userRigthBg">
				  <!--修改头像开始-->
				  <form id="form1" name="form1" runat="server">
                  <table width="100%" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td colspan="2">请您选择游戏中的头像代表，以下是您正在使用的头像：</td>
                      </tr>
                    <tr>
                      <td width="36%" align="right">
                        <asp:HiddenField ID="hfFaceID" runat="server" />
				        <img id="picFace" src="/gamepic/face<%= FaceIDs %>.gif" alt="" />&nbsp;&nbsp;
                      </td>
                      <td width="64%">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="javascript:void(0)" id="btnSwitchFace">查看更多头像</a></td>
                    </tr>
                    <tr>
                      <td>&nbsp;</td>
                      <td>&nbsp;</td>
                    </tr>
                    <tr>
                      <td>&nbsp;</td>
                      <td>
                          <asp:Button ID="btnUpdate" Text="确 定" runat="server" CssClass="bnt lan bold" 
                                onclick="btnUpdate_Click" />
                      </td>
                    </tr>
                  </table>
                  
                  </form>
	              <!--修改头像结束-->
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
