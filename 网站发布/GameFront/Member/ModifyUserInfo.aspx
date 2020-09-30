<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ModifyUserInfo.aspx.cs" Inherits="Game.Web.Member.ModifyUserInfo" %>
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
    
    <script type="text/javascript">
	    function checkEmail(){
	        var regEmail = /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
            if($("#txtEmail").val()!=""){
                if(!regEmail.test($("#txtEmail").val())){
                    alert("Email地址不正确，请重新输入");
                    return false;
                }
            }
            return true;
	    }
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
				<div class="userRigthTitle">会员中心 - 帐号服务 - 修改资料</div>
				<div class="userRigthBg">
				    <!--修改昵称开始-->				    
				    <form id="form1"  runat="server">
	                <table width="100%" cellpadding="0" cellspacing="0">
                    <tr>
                      <td bgcolor="#f7f9fd" colspan="4" align="left"><strong> 　1.帐号资料</strong></td>
                    </tr>
                    <tr>
                      <td colspan="4"><table id="table0" border="0" cellspacing="0" cellpadding="0" width="100%">
                          <tbody>
                            <tr>
                              <td width="17%" align="right">帐号：</td>
                              <td width="28%" align="left"><asp:Label ID="lblAccounts" runat="server"></asp:Label></td>
                              <td width="18%" align="right">ＩＤ号码：</td>
                              <td width="37%" align="left"><asp:Label ID="lblGameID" runat="server"></asp:Label></td>
                            </tr>
                          </tbody>
                      </table></td>
                    </tr>
                    <tr>
                      <td bgcolor="#F7F9FD" colspan="4" align="left"><strong>　2.基本资料</strong></td>
                    </tr>
                    <tr>
                      <td colspan="4"><table border="0" cellspacing="0" cellpadding="0" width="100%">
                          <tbody>
                            <tr>
                              <td width="17%" align="right">性别：</td>
                              <td width="83%" align="left">
                                  <asp:DropDownList ID="ddlGender" runat="server">
                                    <asp:ListItem Text="女性" Value="0"></asp:ListItem>
                                    <asp:ListItem Text="男性" Value="1"></asp:ListItem>
                                  </asp:DropDownList>
                              </td>                              
                            </tr>
                            <tr>
                              <td width="17%" align="right">个性签名：</td>
                              <td width="83%" align="left">
                                <asp:TextBox ID="txtUnderWrite" runat="server" CssClass="text" Width="457"></asp:TextBox>     
                              </td>                              
                            </tr>
                          </tbody>
                      </table></td>
                    </tr>
                    <tr>
                      <td bgcolor="#F7F9FD" colspan="4" align="left"><strong>　3.我的身份资料</strong></td>
                    </tr>
                    <tr>
                      <td colspan="4"><table border="0" cellspacing="0" cellpadding="0" width="100%">
                          <tbody>
                            <tr>
                              <td  align="right">真实姓名：</td>
                              <td width="32%" align="left" colspan="3"><asp:TextBox ID="txtCompellation" runat="server" CssClass="text"></asp:TextBox></td>
                            </tr>     
                            <tr>
                              <td width="17%" align="right">手机号码：</td>
                              <td width="28%" align="left"><asp:TextBox ID="txtMobilePhone" runat="server" CssClass="text"></asp:TextBox></td>
                              <td width="18%" align="right">固定电话：</td>
                              <td width="37%" align="left"><asp:TextBox ID="txtSeatPhone" runat="server" CssClass="text"></asp:TextBox></td>
                            </tr>                       
                            <tr>
				              <td align="right">QQ/MSN：</td>
                              <td align="left"><asp:TextBox ID="txtQQ" runat="server" CssClass="text"></asp:TextBox></td>
                              <td align="right">Email：</td>
                              <td align="left"><asp:TextBox ID="txtEmail" runat="server" CssClass="text"></asp:TextBox></td>
                            </tr>
                            <tr>
                              <td align="right">详细地址：</td>
                              <td align="left" colspan="3"><asp:TextBox ID="txtAddress" runat="server" CssClass="text" Width="457"></asp:TextBox></td>
                            </tr>
                          </tbody>
                      </table></td>
                    </tr>                    
                    <tr>
                      <td bgcolor="#F7F9FD" colspan="4" align="left"><strong>　4.我的其他说明 </strong></td>
                    </tr>
                    <tr>
                      <td colspan="4" align="middle">
                        <asp:TextBox ID="txtUserNote" runat="server" CssClass="mtop13" Width="490" Height="150" TextMode="MultiLine"></asp:TextBox>
                      </td>
                    </tr>
                    <tr>
                      <td colspan="4" align="center">
                        <asp:Button ID="btnUpdate" runat="server" OnClientClick="return checkEmail()" CssClass="bnt lan bold" Text="确 定" 
                              onclick="btnUpdate_Click" />
                      </td>
                    </tr>
                    </table>
                                     
                    </form>
	                <!--修改昵称结束-->
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
