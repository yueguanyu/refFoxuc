<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ApplyPasswordCard.aspx.cs" Inherits="Game.Web.Member.ApplyPasswordCard" %>

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
				  <div class="title f14 bold lan">温馨提示</div>
				   <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                        <tr>
                          <td width="10%" align="right"></td>
                          <td width="90%" class="hui6">
                            1、密保卡图片保存后，建议您立即进行绑卡操作。<br/>
                            2、为了方便使用，可将密保卡打印、手抄或存到手机。<br/>
                            3、尽量不要保存在QQ邮箱、QQ网络硬盘。 <br/>
                            4、有任何问题，请您联系客服中心。<br/>
                          </td>
                        </tr>
                    </table>  
				  <div class="title f14 bold lan">第一步：请保存好您的电子密保卡</div>
	              <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class=".leftTop2 mtop10">
                        <tr align="center" class="bold">
                            <td align="left" class="trBg2" style="padding-left:10px;" colspan="5">序列号：<asp:Label ID="lbSerialNumber" runat="server" Text=""></asp:Label></td>
                        </tr>
                        <tr align="center" class="bold">
                            <td class="trBg" style="width:50px;">&nbsp;</td>
                            <td class="trBg">1</td>
                            <td class="trBg">2</td>
                            <td class="trBg">3</td>
                            <td class="trBg">4</td>                        
                        </tr> 
                        <tr align="center">
                            <td class=".leftTop2 trBg bold" style="width:50px;">A</td>
                            <td class=".leftTop2 trBg1"> <asp:Label ID="lbA1" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"> <asp:Label ID="lbA2" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"> <asp:Label ID="lbA3" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"> <asp:Label ID="lbA4" runat="server" Text=""></asp:Label></td>                        
                        </tr>
                        <tr align="center">
                            <td class=".leftTop2 trBg bold" style="width:50px;">B</td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbB1" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbB2" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbB3" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbB4" runat="server" Text=""></asp:Label></td>                        
                        </tr>
                        <tr align="center">
                            <td class=".leftTop2 trBg bold" style="width:50px;">C</td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbC1" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbC2" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbC3" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbC4" runat="server" Text=""></asp:Label></td>                        
                        </tr>
                        <tr align="center">
                            <td class=".leftTop2 trBg bold" style="width:50px;">D</td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbD1" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbD2" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbD3" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbD4" runat="server" Text=""></asp:Label></td>                        
                        </tr>
                        <tr align="center">
                            <td class=".leftTop2 trBg bold" style="width:50px;">E</td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbE1" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbE2" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbE3" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbE4" runat="server" Text=""></asp:Label></td>                        
                        </tr>
                        <tr align="center">
                            <td class=".leftTop2 trBg bold" style="width:50px;">F</td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbF1" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbF2" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbF3" runat="server" Text=""></asp:Label></td>
                            <td class=".leftTop2 trBg1"><asp:Label ID="lbF4" runat="server" Text=""></asp:Label></td>                        
                        </tr>
                    </table>
                      <table width="100%" border="0" cellspacing="0" cellpadding="0" class="mtop13">                  
                        <tr>
                          <td>
                              <asp:Button ID="Button1" runat="server" Text="图片保存" OnClick="DownloadImg" CssClass="btn lan bold"/></td>
                          <td>&nbsp;</td>
                        </tr>
                      </table>
                  <br />
                  <div class="title f14 bold lan">第二步：请输入以上密保卡对应的坐标数字，并绑定</div>
                   <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class="leftTop2 mtop10">
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
                          <td><asp:Button ID="Button2" runat="server" Text="绑定密保卡" OnClick="BindPasswordCard" CssClass="btn lan bold"/></td>
                          <td>&nbsp;</td>
                        </tr>
                  </table>
                    </asp:Panel>
                    
                   <asp:Panel ID=Panel2 runat="server">
                        <div class="loginHint">
				        <div class="Uright">您已经成功申请了密保卡！</div>
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
