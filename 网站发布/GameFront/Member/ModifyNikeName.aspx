<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ModifyNikeName.aspx.cs" Inherits="Game.Web.Member.ModifyNikeName" %>
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

    <script src="../js/inputCheck.js" type="text/javascript"></script>
    
    <script type="text/javascript">
	    function checkNickname(){
	        if($.trim($("#txtNickName").val())==""){
	            alert("请输入您的新昵称!");
	            $("#txtNickName").focus();
	            return false;
	        }
	        
	        var reg = /^[a-zA-Z0-9_\u4e00-\u9fa5]+$/;
            if(!reg.test($("#txtNickName").val())){
                alert("昵称是由字母、数字、下划线和汉字的组合！");
                return false;
            }
            
            if(byteLength($("#txtNickName").val())<6){
                alert("昵称的长度至少为6个字符");
                return false;
            }
            
            if($("#txtNickName").val().length>31){
                alert("昵称的长度不能超过31个字符");
                return false;
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
				<div class="userRigthTitle">会员中心 - 帐号服务 - 修改昵称</div>
				<div class="userRigthBg">
				    <!--修改资料开始-->				    
				    <form id="form1" name="form1" runat="server">
				    
	                <table width="100%" border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td align="right">当前昵称：</td>
                        <td class="c"><asp:Label ID="lblNickname" runat="server"></asp:Label></td>
                      </tr>
                      <tr>
                        <td align="right">ID 号码：</td>
                        <td><asp:Label ID="lblGameID" runat="server"></asp:Label></td>
                      </tr>
                      <tr>
                        <td align="right">新昵称：</td>
                        <td><asp:TextBox ID="txtNickName" runat="server" CssClass="text width200"></asp:TextBox></td>
                      </tr>                      
                      <tr>
                        <td>&nbsp;</td>
                        <td>
                            <asp:Button ID="btnUpdate" Text="确 定" OnClientClick="return checkNickname();" runat="server" CssClass="bnt lan bold" onclick="btnUpdate_Click" />
                        </td>
                      </tr>                      
                    </table>   
                                     
                    </form>
	                <!--修改资料结束-->
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
