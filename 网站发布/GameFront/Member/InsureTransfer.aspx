<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="InsureTransfer.aspx.cs" Inherits="Game.Web.Member.InsureTransfer" %>
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
            if($("#radType1").attr("checked")==true){
                $("#lblTitle").attr("innerText","转入昵称：");
            }else{
                $("#lblTitle").attr("innerText","转入ＩＤ：");
            }
            
            $("#radType1").bind("click",function(){$("#lblTitle").attr("innerText","转入昵称：");})
            $("#radType2").bind("click",function(){$("#lblTitle").attr("innerText","转入ＩＤ：");})
            
            //页面验证
            $.formValidator.initConfig({formid:"form1",alertmessage:true,onerror:function(msg){alert(msg)}});
            $("#txtUser").formValidator().inputValidator({min:1,empty:{leftempty:false,rightempty:false,emptyerror:"两边不能有空符号！"},onerror:"转入用户昵称不能为空！"})
            $("#txtScore").formValidator().inputValidator({min:1000,type:"number",onerror:"抱歉，您每笔转出数目最少 1000 金币！"})
                .regexValidator({regexp:"intege1",datatype:"enum",onerror:"转出的金币必须为整数，并且不能大于保险柜数目！"}); 
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
				<div class="userRigthTitle">会员中心 - 保险柜服务 - 转账</div>
				<div class="userRigthBg">
				<!--转帐开始-->
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
			    <div class="title f14 bold lan">金币转账</div>
	            <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
	              <tr>
                    <td width="400px" colspan="2" align="center" style="padding-left:10px;">
                        <input type="radio" id="radType1" name="radType" value="0" checked="true" runat="server" /><label for="radType1" hidefocus="true">按用户昵称</label>
                        <input type="radio" id="radType2" name="radType" value="1" runat="server" /><label for="radType2" hidefocus="true">按ＩＤ号码</label>
                    </td>
                  </tr>
                  <tr>
                    <td width="20%" align="right"><label id="lblTitle"></label></td>
                    <td width="80%"><asp:TextBox ID="txtUser" runat="server" CssClass="text"></asp:TextBox></td>
                  </tr>
                  <tr>
                      <td width="20%" align="right">转账金额：</td>
                      <td width="80%"><asp:TextBox ID="txtScore" runat="server" CssClass="text"></asp:TextBox></td>
                    </tr>
                  <tr>
                      <td align="right">保险柜密码：</td>
                      <td><asp:TextBox ID="txtInsurePass" runat="server" TextMode="Password" CssClass="text" Width="149"></asp:TextBox></td>
                    </tr>
                  <tr>
                    <td align="right">备注信息：</td>
                    <td><asp:TextBox ID="txtNote" runat="server" CssClass="beizhu" TextMode="MultiLine"></asp:TextBox></td>
                  </tr>
                  <tr>
                    <td>&nbsp;</td>
                    <td>
                        <asp:Button ID="btnUpdate" Text="确定转账" runat="server" CssClass="bnt lan bold" 
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
                        1、您可以填写接收人的“用户昵称”或“游戏ID”进行赠送。<br>
                        2、您赠送给接收人的金币会在您保险柜的金币上面扣除。<br>
                        3、每次赠送，系统收取一定比例的服务费。<br>
                      </td>
                    </tr>
                  </table>       
                </form>
	            <!--转帐结束-->
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
