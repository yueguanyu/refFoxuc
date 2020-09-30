<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ModifyLogonPass.aspx.cs" Inherits="Game.Web.Member.ModifyLogonPass" %>
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
    
    <script type="text/javascript">
        $(document).ready(function(){
            //页面验证
            $.formValidator.initConfig({formid:"form1",alertmessage:false});
            $("#txtOldPass").formValidator({onshow:"请输入原始密码！",onfocus:"请输入密码，至少需要6位！"})
                .inputValidator({min:6,onerror:"你输入的密码非法,请确认"});
            $("#txtNewPass").formValidator({onshow:"请输入密码，至少需要6位！",onfocus:"请输入密码，至少需要6位！"})
                .inputValidator({min:6,onerror:"你输入的密码非法,请确认"});
            $("#txtNewPass2").formValidator({onshow:"确认密码必须和新密码完全一致！",onfocus:"请输入密码，至少需要6位！"})
                .compareValidator({desid:"txtNewPass",operateor:"=",onerror:"两次密码不一致,请确认"});
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
				<div class="userRigthTitle">会员中心 - 帐号服务 - 修改登录密码</div>
				<div class="userRigthBg">
				  <!--修改登录密码开始-->
				  <form id="form1" name="form1" runat="server" >
                  <table width="100%" border="0" cellspacing="0" cellpadding="0">
                  <tr>
                    <td width="200" align="right">原密码：</td>
                    <td>
                        <asp:TextBox ID="txtOldPass" runat="server" TextMode="Password" CssClass="text"></asp:TextBox>&nbsp;
                        <span id="txtOldPassTip"></span>
                    </td>                    
                  </tr>
                  <tr>
                    <td align="right">新密码：</td>
                    <td>
                        <asp:TextBox ID="txtNewPass" runat="server" TextMode="Password" CssClass="text"></asp:TextBox>&nbsp;
                        <span id="txtNewPassTip"></span>
                    </td>
                  </tr>
                  <tr>
                    <td align="right">确认新密码：</td>
                    <td>
                        <asp:TextBox ID="txtNewPass2" runat="server" TextMode="Password" CssClass="text"></asp:TextBox>&nbsp;
                        <span id="txtNewPass2Tip"></span>
                    </td>
                  </tr>
                  <tr>
                    <td>&nbsp;</td>
                    <td>
                        <asp:Button ID="btnUpdate" Text="确 定" runat="server" CssClass="bnt lan bold" 
                                onclick="btnUpdate_Click" />
                        <input name="button" type="reset" class="bnt lan bold" value="取 消" hidefocus="true"/>
                    </td>
                  </tr>      
                  </table>
                  
                  </form>
	              <!--修改登录密码结束-->
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
