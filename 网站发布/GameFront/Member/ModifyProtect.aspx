<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ModifyProtect.aspx.cs" Inherits="Game.Web.Member.ModifyProtect" %>
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
            $.formValidator.initConfig({formid:"form2",alertmessage:false});
            $("#ddlQuestion1").formValidator({onshow:" ",onfocus:"请选择问题"}).inputValidator({min:1,onerror: "你是不是忘记选择问题了"})
                .functionValidator({
                    fun:function(val,elem)
                    {
                        if(val != $("#ddlQuestion2").val()&&val !=$("#ddlQuestion3").val())
                        {
                            return true;
                        }
                        else
                        {
                            return "不能选择相同的问题";
                        }
                    }
                });
            $("#txtMResponse1").formValidator({onshow:" ",onfocus:"答案至少2~16个英文或中文字符"}).inputValidator({min:2,max:16,onerror:"你输入的答案非法"})
                .regexValidator({regexp:"usernamecn",datatype:"enum",onerror:"答案格式不正确"});
            $("#ddlQuestion2").formValidator({onshow:" ",onfocus:"请选择问题"}).inputValidator({min:1,onerror: "你是不是忘记选择问题了"})
                .functionValidator({
                    fun:function(val,elem)
                    {
                        if(val != $("#ddlQuestion1").val()&&val !=$("#ddlQuestion3").val())
                        {
                            return true;
                        }
                        else
                        {
                            return "不能选择相同的问题";
                        }
                    }
                });
            $("#txtMResponse2").formValidator({onshow:" ",onfocus:"答案至少2~16个英文或中文字符"}).inputValidator({min:2,max:16,onerror:"你输入的答案非法"})
                .regexValidator({regexp:"usernamecn",datatype:"enum",onerror:"答案格式不正确"});
            $("#ddlQuestion3").formValidator({onshow:" ",onfocus:"请选择问题"}).inputValidator({min:1,onerror: "你是不是忘记选择问题了"})
                .functionValidator({
                    fun:function(val,elem)
                    {
                        if(val != $("#ddlQuestion1").val()&&val !=$("#ddlQuestion2").val())
                        {
                            return true;
                        }
                        else
                        {
                            return "不能选择相同的问题";
                        }
                    }
                });
            $("#txtMResponse3").formValidator({onshow:" ",onfocus:"答案至少2~16个英文或中文字符"}).inputValidator({min:2,max:16,onerror:"你输入的答案非法"})
                .regexValidator({regexp:"usernamecn",datatype:"enum",onerror:"答案格式不正确"});
            $("#txtSafeEmail").formValidator({onshow:" ",onfocus:"请输入邮箱号码"}).inputValidator({min:6,max:100,onerror:"你输入的邮箱长度非法"})
                .functionValidator({fun:isEmail,onerror:"输入的格式不对"});
            $("#txtSafeEmail2").formValidator({onshow:" ",onfocus:"请输入邮箱号码"}).inputValidator({min:6,max:100,onerror:"你输入的邮箱长度非法"})
                .compareValidator({desid:"txtSafeEmail",operateor:"=",onerror:"两次填写的邮箱号码不一致"});
                
            
            
            $("#txtResponse1").blur(function(){ checkResponse1(); });
            $("#txtResponse2").blur(function(){ checkResponse2(); });
            $("#txtResponse3").blur(function(){ checkResponse3(); });
            
            $("#btnConfirm").click(function (){
                return checkInput();
            });
        })
        
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
				<div class="userRigthTitle">会员中心 - 帐号安全 - 修改密码保护</div>
				<div class="userRigthBg">
				<!--修改密码保护开始-->
								  
	              <!--第一步开始-->
	              <form id="form1" name="form1" runat="server">
                  <table width="100%" cellpadding="0" cellspacing="0">
                    <tr class="bold">
                      <td width="26%" align="right">第一步：</td>
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
                        <asp:Button ID="btnConfirm" Text="确 定" runat="server" CssClass="bnt lan bold" 
                                onclick="btnConfirm_Click" />
                        <input name="button2432" type="reset" class="bnt lan bold" value="重 置" hidefocus="true"/>
                      </td>
                      </tr>
                  </table>
                  </form>
                  <!--第一步结束-->
                  
                  <div class="loginHint" id="divRight" runat="server">

				  </div>
                  
	              <!--第二步开始-->
	              <form id="form2" name="form2" runat="server">
                  <table width="100%" border="0" cellspacing="0" cellpadding="0">  
                    <tr class="bold">
                      <td width="26%" align="right">第二步：</td>
                      <td colspan="2" align="left">请填写新的帐号保护信息</td>
                    </tr>                       
                    <tr bgcolor="#F7F9FD">
                      <td width="180" align="right">问题一：</td>
                      <td width="180">
                        <asp:DropDownList ID="ddlQuestion1" runat="server" Width="170">
                            
                        </asp:DropDownList>
                      </td>
                      <td><span id="ddlQuestion1Tip"></span></td>
                    </tr>
                    <tr>
                      <td align="right">答案：</td>
                      <td><asp:TextBox ID="txtMResponse1" runat="server" Width="162" CssClass="text"></asp:TextBox></td>
                      <td><span id="txtMResponse1Tip"></span></td>
                    </tr>
                    <tr bgcolor="#F7F9FD">
                      <td align="right">问题二：</td>
                      <td>
                        <asp:DropDownList ID="ddlQuestion2" runat="server" Width="170">
                            
                        </asp:DropDownList>
                      </td>
                      <td><span id="ddlQuestion2Tip"></span></td>
                    </tr>
                    <tr>
                      <td align="right">答案：</td>
                      <td><asp:TextBox ID="txtMResponse2" runat="server" Width="162" CssClass="text"></asp:TextBox></td>
                      <td><span id="txtMResponse2Tip"></span></td>
                    </tr>
                    <tr bgcolor="#F7F9FD">
                      <td align="right">问题三：</td>
                      <td>
                        <asp:DropDownList ID="ddlQuestion3" runat="server" Width="170">
                            
                        </asp:DropDownList>
                      </td>
                      <td><span id="ddlQuestion3Tip"></span></td>
                    </tr>
                    <tr>
                      <td align="right">答案：</td>
                      <td><asp:TextBox ID="txtMResponse3" runat="server" CssClass="text" Width="162"></asp:TextBox></td>
                      <td><span id="txtMResponse3Tip"></span></td>
                    </tr>                    
                    <tr>
                      <td align="right">电子邮箱：</td>
                      <td><asp:TextBox ID="txtSafeEmail" runat="server" CssClass="text" Width="162"></asp:TextBox></td>
                      <td><span id="txtSafeEmailTip"></span></td>
                    </tr>
                    <tr bgcolor="#F7F9FD">
                      <td align="right">确认邮箱：</td>
                      <td><asp:TextBox ID="txtSafeEmail2" runat="server" CssClass="text" Width="162"></asp:TextBox></td>
                      <td><span id="txtSafeEmail2Tip"></span></td>
                    </tr>
                    <tr>
                      <td>&nbsp;</td>
                      <td>
                        <asp:Button ID="btnUpdate" Text="确 定" runat="server" CssClass="bnt lan bold" 
                                onclick="btnUpdate_Click" />
                        <input name="button243" type="reset" class="bnt lan bold" value="重 置" hidefocus="true"/>
                      </td>
                      <td>&nbsp;</td>
                    </tr>
                  </table>
                  
                  </form>
	              <!--第二步结束-->
                <!--修改密码保护结束-->
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
