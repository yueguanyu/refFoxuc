<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ConvertMedal.aspx.cs" Inherits="Game.Web.Member.ConvertMedal" %>

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

    <script src="../js/jquery-1.5.2.min.js" type="text/javascript"></script>
    <script src="/JS/formValidator.js" type="text/javascript"></script>
    <script src="/JS/formValidatorRegex.js" type="text/javascript"></script>
    
    <script type="text/javascript">
        $(document).ready(function(){
            //页面验证
           $.formValidator.initConfig({formid:"form1",alertmessage:true,onerror:function(msg){alert(msg)}});
            $("#txtPresent").formValidator()
                .regexValidator({regexp:"intege1",datatype:"enum",onerror:"兑换的奖牌数必须为正整数！"});            
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
				<div class="userRigthTitle">会员中心 - 保险柜服务 - 奖牌兑换</div>
				<div class="userRigthBg">
				<!--兑换开始-->
				
	            <div class="title f14 bold lan">奖牌兑换</div>
	            <br />
	            <form id="form1" name="form1" runat="server">
	            
	            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5">
	                <tr>
	                <td height="139" background="/images/contestBg1.jpg">
	                    <table width="96%" border="0" align="center" cellpadding="0" cellspacing="0" >
                          <tr>
                            <td width="25%" align="right" class="lineBottom">ID：</td>
                            <td width="27%" class="lineBottom"><asp:Label ID="lblGameID" runat="server"></asp:Label></td>
                            <td width="17%" align="right" class="lineBottom">帐　号：</td>
                            <td width="31%" class="lineBottom"><asp:Label ID="lblAccounts" runat="server"></asp:Label></td>
                          </tr>
                          <tr>
                            <td align="right">剩余奖牌：</td>
                            <td  class="hong bold"><asp:Label ID="lblMedals" runat="server"></asp:Label></td>
                            <td align="right" class="lineBottom">保险柜金币值：</td>
                            <td  class="lineBottom hong bold"><asp:Label ID="lblInsureScore" runat="server" Text="0"></asp:Label></td>
                          </tr>
                        </table>
	                </td>
	                </tr>
	            </table>
	            <br />
	            
	            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                  <tr>
                    <td>
                    <table width="100%" border="0" cellspacing="0" cellpadding="0" class="loginHint">
                      <tr class="bold hui3">
                        <td width="28%" align="right">将</td>
                        <td width="26%" align="center"><asp:TextBox ID="txtMedals" runat="server" CssClass="text"></asp:TextBox></td>
                        <td width="46%" align="left">奖牌兑换成游戏币</td>
                      </tr>
                    </table>
                    </td>
                  </tr>
                  <tr>
                    <td align="center"><asp:Button ID="btnUpdate" Text="兑 换" runat="server" CssClass="bnt lan bold" 
                                onclick="btnUpdate_Click" />
                    </td>
                  </tr>
                </table>
                </form>
                <br />
                <div class="title f14 bold lan">温馨提示</div>
                <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                   <tr>
                     <td width="10%" align="right"></td>
                     <td width="90%" class="hui6">
                        1、奖牌兑换比例为1：10。<br />
                        2、奖牌直接兑换到保险柜金币。<br />
                        3、有任何问题，请您联系客服中心<br />
                     </td>
                   </tr>
                </table>     
                
	            <!--兑换结束-->
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