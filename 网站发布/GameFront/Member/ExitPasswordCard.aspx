<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ExitPasswordCard.aspx.cs" Inherits="Game.Web.Member.ExitPasswordCard" %>

<%@ Import Namespace="Game.Facade" %>
<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Info" Src="~/Themes/Standard/User_Info.ascx" %>
<%@ Register TagPrefix="qp" TagName="Logon" Src="~/Themes/Standard/User_Logon.ascx" %>
<%@ Register TagPrefix="qp" TagName="UserSidebar" Src="~/Themes/Standard/User_Sidebar.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" >
<head id=Head1 runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/user_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/game_layout.css" />   

    <script src="/JS/jquery-1.5.2.min.js" type="text/javascript"></script> 
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
				<div class="userRigthTitle">会员中心 - 账户安全 - 密保卡</div>
				<div class="userRigthBg">
				<!--存款开始-->
				  <form name="form1" id="form1" runat="server">
                   <asp:Panel ID=Panel1 runat="server">
                       <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class="leftTop2 mtop10">
                        <tr align="center" class="bold">
                            <td align="left" class="trBg2" style="padding-left:10px;" colspan="4">序列号：<asp:Label ID="lbSerialNumber" runat="server" Text=""></asp:Label></td>
                        </tr>
                        <tr align="center" class="bold">
                        <td class=".leftTop2 trBg">&nbsp;</td>
                        <td class=".leftTop2 trBg"><asp:Label ID="lbNumber1" runat="server" Text=""></asp:Label></td>
                        <td class=".leftTop2 trBg"><asp:Label ID="lbNumber2" runat="server" Text=""></asp:Label></td>
                        <td class=".leftTop2 trBg"><asp:Label ID="lbNumber3" runat="server" Text=""></asp:Label></td>                       
                        </tr>
                       <tr align="center">
                        <td class="leftTop2 trBg bold" style="width:50px;">坐标码</td>
                        <td class="leftTop2 trBg1"><asp:TextBox ID="txtNumber1" runat="server" CssClass="text"></asp:TextBox></td>
                        <td class="leftTop2 trBg1"><asp:TextBox ID="txtNumber2" runat="server" CssClass="text"></asp:TextBox></td>            
                        <td class="leftTop2 trBg1"><asp:TextBox ID="txtNumber3" runat="server" CssClass="text"></asp:TextBox></td>                           
                       </tr> 
                  </table> 
                     <table width="100%" border="0" cellspacing="0" cellpadding="0" class="mtop13">                  
                        <tr>
                          <td><asp:Button ID="Button2" runat="server" Text="取消密保卡" OnClick="ClearPasswordCard" CssClass="btn lan bold"/></td>
                          <td>&nbsp;</td>
                        </tr>
                  </table>   
				   </asp:Panel>
				   <asp:Panel ID=Panel2 runat="server">
				        <div class="loginHint" id="divRight" runat="server">
	                        <div class="Uerror">您还未申请密保卡！</div>
	                    </div>
	               </asp:Panel>
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