<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="MoorMachine.aspx.cs" Inherits="Game.Web.Member.MoorMachine" %>
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
        $(document).ready(function(){
            $("#txtResponse1").blur(function(){ checkResponse1(); });
            $("#txtResponse2").blur(function(){ checkResponse2(); });
            $("#txtResponse3").blur(function(){ checkResponse3(); });
            
            $("#btnUpdate").click(function (){
                return checkInput();
            });
        });
        
        function hintMessage(hintObj,error,message){
            //删除样式
            if(error=="error"){
	            $("#"+hintObj+"").removeClass("onCorrect");
	            $("#"+hintObj+"").removeClass("onFocus");
	            $("#"+hintObj+"").addClass("onError");
	        }else if(error=="right"){
	            $("#"+hintObj+"").removeClass("onError");
	            $("#"+hintObj+"").removeClass("onFocus");
	            $("#"+hintObj+"").addClass("onCorrect");
    	    }else if(error=="info"){
    	        $("#"+hintObj+"").removeClass("onError");
	            $("#"+hintObj+"").addClass("onFocus");
	            $("#"+hintObj+"").removeClass("onCorrect");
    	    }
        	
	        $("#"+hintObj+"").html(message);
        }
        
        function checkResponse1(){
            if($("#txtResponse1").val()==""){
                hintMessage("txtResponse1Tips","error","请输入您的密保答案");
                return false;
            }
            hintMessage("txtResponse1Tips","right","");
            return true;
        }
        function checkResponse2(){    
            if($("#txtResponse2").val()==""){
                hintMessage("txtResponse2Tips","error","请输入您的密保答案");
                return false;
            }
            hintMessage("txtResponse2Tips","right","");
            return true;
        }
        function checkResponse3(){
            if($("#txtResponse3").val()==""){
                hintMessage("txtResponse3Tips","error","请输入您的密保答案");
                return false;
            }
            hintMessage("txtResponse3Tips","right","");
            return true;
        }
        
        function checkInput(){
            if(!checkResponse1()){$("#txtResponse1").focus(); return false; }
            if(!checkResponse2()){$("#txtResponse2").focus(); return false; }
            if(!checkResponse3()){$("#txtResponse3").focus(); return false; }
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
				<div class="userRigthTitle">会员中心 - 会员服务 - 固定机器</div>
				<div class="userRigthBg">
				<!--固定机器开始-->
				
                <form id="form1" name="form1" runat="server">
                  <table width="100%" cellpadding="0" cellspacing="0">
                    <tr class="bold">
                      <td width="26%" align="right"></td>
                      <td colspan="2" align="left">请回答下面问题</td>
                    </tr>                    
                    <tr bgcolor="#f7f9fd">
                      <td align="right">问题：</td>
                      <td colspan="2" align="left"><asp:Label ID="lblQuestion1" runat="server"></asp:Label></td>
                    </tr>                   
                    <tr>
                      <td height="40" align="right">答案：</td>
                      <td width="27%" align="left"><asp:TextBox ID="txtResponse1" runat="server" CssClass="text"></asp:TextBox></td>
                      <td width="47%" align="left"><span class="tipsM" id="txtResponse1Tips"></span></td>
                    </tr>
                    <tr bgcolor="#f7f9fd">
                      <td align="right">问题：</td>
                      <td colspan="2" align="left"><asp:Label ID="lblQuestion2" runat="server"></asp:Label></td>
                    </tr>                   
                    <tr>
                      <td height="40" align="right">答案：</td>
                      <td width="27%" align="left"><asp:TextBox ID="txtResponse2" runat="server" CssClass="text"></asp:TextBox></td>
                      <td width="47%" align="left"><span class="tipsM" id="txtResponse2Tips"></span></td>
                    </tr>
                    <tr bgcolor="#f7f9fd">
                      <td align="right">问题：</td>
                      <td colspan="2" align="left"><asp:Label ID="lblQuestion3" runat="server"></asp:Label></td>
                    </tr>                   
                    <tr>
                      <td height="40" align="right">答案：</td>
                      <td width="27%" align="left"><asp:TextBox ID="txtResponse3" runat="server" CssClass="text"></asp:TextBox></td>
                      <td width="47%" align="left"><span class="tipsM" id="txtResponse3Tips"></span></td>
                    </tr>                                    
                    <tr>
                      <td height="40" align="right">&nbsp;</td>
                      <td colspan="2" align="left">
                        <asp:Button ID="btnUpdate" Text="申请绑定" runat="server" CssClass="bnt lan bold" 
                                onclick="btnUpdate_Click" />
                        <input name="button2432" type="reset" class="bnt lan bold" value="重置" hidefocus="true"/>
                      </td>
                    </tr>
                  </table>
                </form>
                
	            <!--固定机器结束-->
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
