<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Register.aspx.cs" Inherits="Game.Web.Register" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    <meta name="rating" content="general" />
    <meta name="robots" content="all" />
    <link rel="stylesheet" type="text/css" href="style/layout.css" />
    <link rel="stylesheet" type="text/css" href="style/global.css" />
    <link rel="stylesheet" type="text/css" href="style/reg_layout.css" />
    <script src="js/jquery-1.5.2.min.js"type="text/javascript"></script>
    <script src="JS/passwordStrength.js" type="text/javascript"></script>
    <script src="js/inputCheck.js" type="text/javascript"></script>

    <script src="js/lhgdialog/lhgcore.min.js" type="text/javascript"></script>
    <script src="js/lhgdialog/lhgdialog.js" type="text/javascript"></script>
    <link href="js/lhgdialog/lhgdialog.css" rel="stylesheet" type="text/css" />
    
    <script type="text/javascript">
        function UpdateImage(){
            document.getElementById("picVerifyCode").src="ValidateImage.aspx?r="+Math.random();
        }
        
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
        
        function checkAccounts(){
            if($.trim($("#txtAccounts").val())==""){
                hintMessage("txtAccountsTip","error","请输入您的用户名");
                return false;
            }
            var reg = /^[a-zA-Z0-9_\u4e00-\u9fa5]+$/;
            if(!reg.test($("#txtAccounts").val())){
                hintMessage("txtAccountsTip","error","由字母、数字、下划线和汉字的组合！");
                return false;
            }
            
            if($("#txtAccounts").val().length<6 || $("#txtAccounts").val().length>12){
                hintMessage("txtAccountsTip","error","用户名的长度为6-12个字符");
                return false;
            }
            hintMessage("txtAccountsTip","right","此用户名可以注册!");
            return true;
        }
        
        function checkNickName(){
            if($.trim($("#txtNickname").val())==""){
                hintMessage("txtNicknameTip","error","请输入您的昵称");
                return false;
            }
            var reg = /^[a-zA-Z0-9_\u4e00-\u9fa5]+$/;
            if(!reg.test($("#txtNickname").val())){
                hintMessage("txtNicknameTip","error","由字母、数字、下划线和汉字的组合！");
                return false;
            }

            if ( $("#txtNickname").val().length < 6 || $("#txtNickname").val().length > 12) {
                hintMessage("txtNicknameTip","error","昵称的长度至少为6个字符");
                return false;
            }
            if($("#txtNickname").val().length>31){
                hintMessage("txtNicknameTip","error","昵称的长度不能超过31个字符");
                return false;
            }
            hintMessage("txtNicknameTip","right","");
            return true;
        }
        
        var isExitsUserName = true;
        function checkUserName(){
            $.ajax({
                async:false,
                contentType: "application/json",
                url: "WS/WSAccount.asmx/CheckName",
                data: "{userName:'"+ $("#txtAccounts").val() +"'}",
                type:"POST",
                dataType: "json",
                success:function(json){
                    json = eval("("+json.d+")");
                    
                    if(json.success=="error"){
                        hintMessage("txtAccountsTip","error",json.msg);
                        isExitsUserName = true;
                        return;
                    }else if(json.success=="success"){
                        hintMessage("txtAccountsTip","right","此用户名可以注册!");
                        isExitsUserName = false;
                        return;
                    }
                },
                error:function(err,ex){
                    //alert(err.responseText);
                }
            });
        }
        
        function checkLoginPass(){
            if($("#txtLogonPass").val()==""){
                hintMessage("txtLogonPassTip","error","请输入您的登录密码");
                return false;
            }
            if($("#txtLogonPass").val().length<6||$("#txtLogonPass").val().length>32){
                hintMessage("txtLogonPassTip","error","登录密码长度为6到32个字符");
                return false;
            }
            hintMessage("txtLogonPassTip","right","");
            return true;
        }
        
        function checkLoginConPass(){
            if($("#txtLogonPass2").val()==""){
                 hintMessage("txtLogonPass2Tip","error","请输入登录确认密码");
                 return false;
            }
            if($("#txtLogonPass2").val() != $("#txtLogonPass").val()){
                hintMessage("txtLogonPass2Tip","error","登录确认密码与原密码不同，请重新输入");
                return false;
            }
            hintMessage("txtLogonPass2Tip","right","");
            return true;
        }
        
        function checkInsurePass(){
            if($("#txtInsurePass1").val()==""){
                hintMessage("txtInsurePass1Tip","error","请输入您的银行密码");
                return false;
            }
            if($("#txtInsurePass1").val().length<6||$("#txtInsurePass1").val().length>32){
                hintMessage("txtInsurePass1Tip","error","银行密码长度为6到32个字符");
                return false;
            }
            hintMessage("txtInsurePass1Tip","right","");
            return true;
        }
        function checkInsureConPass(){
            if($("#txtInsurePass2").val()==""){
                hintMessage("txtInsurePass2Tip","error","请输入您的银行确认密码");
                return false;
            }
            if($("#txtInsurePass2").val() != $("#txtInsurePass1").val()){
                hintMessage("txtInsurePass2Tip","error","登录确认密码与原密码不同，请重新输入");
                return false;
            }
            hintMessage("txtInsurePass2Tip","right","");
            return true;
        }
        
        function checkCompellation(){
            if($.trim($("#txtCompellation").val())!=""){
                if($("#txtCompellation").val().length>16){
                    hintMessage("txtCompellationTip","error","真实姓名长度不能大于16个字符");
                    return false;
                }
            }
            hintMessage("txtCompellationTip","right","");
            return true;
        }
        function checkCard(){
            if($.trim($("#txtPassPortID").val())!=""){
                if(!isIDCard(document.getElementById("txtPassPortID"))){
                    hintMessage("txtPassPortIDTip","error","您的身份证号错误，请重新输入");
                    return false;
                }
            }
            hintMessage("txtPassPortIDTip","right","");
            return true;
        }
        function checkVerifyCode(){
            if($("#txtCode").val()==""){
                hintMessage("txtCodeTip","error","请输入验证码");
                return false;
            }
            hintMessage("txtCodeTip","right","");
            return true;
        }
        function checkSpreader(){
            if($("#txtSpreader").val()!=""){
                 if($("#txtSpreader").val().length>31){
                    hintMessage("txtSpreaderTip","error","推荐人账号长度不能大于31个字符");
                    return false;
                }
            }
            hintMessage("txtSpreaderTip","right","");
            return true;
        }
        
        function checkService(){
            if(!$("#chkAgree").attr("checked")){
                hintMessage("chkAgreeTip","error","请选中已阅读并同意服务条款");
                return false;
            }
            hintMessage("chkAgreeTip","right","");
            return true;
        }
        
        function checkInput(){
            if(!checkAccounts()){
                $("#txtAccounts").focus();return false; 
            }else{
                if(isExitsUserName){
                    $("#txtAccounts").focus();
                    hintMessage("txtAccountsTip","error","很遗憾，该用户名已经被注册，请您另选一个");
                    return false;
                }
            }
            
            if(!checkNickName()){$("#txtNickname").focus(); return false; }
            if(!checkLoginPass()){$("#txtLogonPass").focus(); return false; }
            if(!checkLoginConPass()){$("#txtLogonPass2").focus(); return false; }
            if(!checkInsurePass()){ $("#txtInsurePass1").focus(); return false; }
            if(!checkInsureConPass()){$("#txtInsurePass2").focus(); return false; }
            if(!checkSpreader()){$("#txtSpreader").focus(); return false; }
            if(!checkCompellation()){$("#txtCompellation").focus(); return false; }
            if(!checkCard()){ $("#txtPassPortID").focus(); return false; }
            if(!checkVerifyCode()){ $("#txtCode").focus(); return false;}
            if(!checkService()){ return false;}
        }
            
        $(document).ready(function(){
            $("#txtAccounts").blur(function(){
                if(checkAccounts()){
                    checkUserName();
                }
            });
            
            $("#txtNickname").blur(function(){ checkNickName(); });
            $("#txtCompellation").blur(function(){ checkCompellation(); });
            $("#txtLogonPass").blur(function(){ checkLoginPass(); });
            $("#txtLogonPass2").blur(function(){ checkLoginConPass(); });
            
            $("#txtInsurePass1").blur(function(){ checkInsurePass(); });
            $("#txtInsurePass2").blur(function(){ checkInsureConPass(); });
            
            $("#txtPassPortID").blur(function(){ checkCard(); });
            $("#txtSpreader").blur(function(){ checkSpreader(); });
            $("#txtCode").blur(function(){ checkVerifyCode(); });
            
            $("#btnRegister").click(function (){
                return checkInput();
            });
            
            //密码强度
            strongRankBind($("#txtLogonPass"), $("#lblLogonPassSafeRank"));
            strongRankBind($("#txtInsurePass1"), $("#lblInsurePassSafeRank"));
            
            //弹出页面
            
            J('#btnSwitchFace').dialog({ id: 'winUserfaceList', title: '更换头像', width:540, height:385, page: '/FaceList.aspx',rang: true,cover: true });
        });
    </script>
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<form id="form1" runat="server">
<div class="main">
	<div class="regBody">
	  <div class="regTitle"></div>
	  <div class="regBg">
			<!--创建帐号开始-->
			<div class="title lan f14 bold">创建帐号</div>
			<div >
			<table width="100%" border="0" cellpadding="0" cellspacing="0" class="mtop13">
			  <tr>
				<td width="230" align="right" class="f14">用&nbsp;&nbsp;户&nbsp;名：</td>
				<td width="206"><asp:TextBox runat="server" ID="txtAccounts" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
			    <td width="304"><span id="txtAccountsTip"></span></td>
			  </tr>
			  <tr>
				<td width="230" align="right" class="f14">昵&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;称：</td>
				<td width="206"><asp:TextBox ID="txtNickname" runat="server" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
			    <td width="304"><span id="txtNicknameTip" class="tipsRight"></span></td>
			  </tr>
			  <tr>
				<td width="230" align="right" class="f14">形&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;象：</td>
				<td width="206">
				    <asp:HiddenField  runat="server" Value="0" ID="hfFaceID" />
				    <img id="picFace" src="/gamepic/face0.gif" alt="" />&nbsp;&nbsp;
				    <input id="btnSwitchFace" type="button" value="更换头像" class="bnt lan" />
				</td>
				<td width="304">&nbsp;</td>
			  </tr>
			  <tr>
				<td width="230" align="right" class="f14">性&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;别：</td>
				<td width="206">
                    <asp:DropDownList ID="ddlGender" runat="server" CssClass="width206" TabIndex="4">
                        <asp:ListItem Selected="True" Text="女" Value="2"></asp:ListItem>
                        <asp:ListItem Text="男" Value="1"></asp:ListItem>
                    </asp:DropDownList>
				</td>
			    <td></td>
			  </tr>			  
			</table>
			</div>
			<!--创建帐号结束-->
			
			<!--登录密码开始-->
			<div class="title lan f14 bold mtop13">登录密码</div>
			<div >
			<table width="100%" border="0" cellpadding="0" cellspacing="0" class="mtop13">	
			  <tr>
				<td width="230" align="right" class="f14">安全等级：</td>
				<td width="206"><span class="safe0" id="lblLogonPassSafeRank"></span></td>
			    <td width="304"></td>
			  </tr>
			  <tr>
				<td align="right" class="f14">密&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;码：</td>
				<td><asp:TextBox ID="txtLogonPass" runat="server" TextMode="Password" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
			    <td><span id="txtLogonPassTip"></span></td>
			  </tr>
			  <tr>
				<td align="right" class="f14">确认密码：</td>
				<td><asp:TextBox ID="txtLogonPass2" runat="server" TextMode="Password" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
			    <td><span id="txtLogonPass2Tip"></span></td>
			  </tr>
			  <tr>
				<td width="230" align="right" class="f14">安全等级：</td>
				<td width="206"><span class="safe0" id="lblInsurePassSafeRank"></span></td>
			    <td width="304"></td>
			  </tr> 
			  <tr>
				<td align="right" class="f14">保险柜密码：</td>
				<td><asp:TextBox ID="txtInsurePass1" runat="server" TextMode="Password" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
			    <td><span id="txtInsurePass1Tip"></span></td>
			  </tr>
			  <tr>
				<td align="right" class="f14">确认密码：</td>
				<td><asp:TextBox ID="txtInsurePass2" runat="server" TextMode="Password" CssClass="text width200"></asp:TextBox><span class="hong f14">&nbsp;*</span></td>
			    <td><span id="txtInsurePass2Tip"></span></td>
			  </tr> 			  
			</table>
			</div>
			<!--登录密码结束-->	
			
			<!--推荐人开始-->
			<div class="title lan f14 bold mtop13">推荐人</div>
			<div >
			<table width="100%" border="0" cellpadding="0" cellspacing="0" class="mtop13">	
			  <tr>
			    <td width="220" align="right" class="f14">推&nbsp;&nbsp;荐&nbsp;人：</td>
			    <td width="206"><asp:TextBox ID="txtSpreader" runat="server" CssClass="text width200"></asp:TextBox></td>
			    <td width="304"><span id="txtSpreaderTip"></span></td>
		      </tr>	  	
		      <tr>
			    <td width="220" align="right" class="f14">真实姓名：</td>
			    <td width="206"><asp:TextBox ID="txtCompellation" runat="server" CssClass="text width200"></asp:TextBox></td>
			    <td width="304"><span id="txtCompellationTip"></span></td>
		      </tr>	
		      <tr>
			    <td width="220" align="right" class="f14">身份证号：</td>
			    <td width="206"><asp:TextBox ID="txtPassPortID" runat="server" CssClass="text width200"></asp:TextBox></td>
			    <td><span id="txtPassPortIDTip"></span></td>
		      </tr>	  
			  <tr>
			    <td align="right" class="f14">验&nbsp;&nbsp;证&nbsp;码：</td>
			    <td colspan="2">
			        <asp:TextBox ID="txtCode" runat="server" CssClass="text width110"></asp:TextBox>
			        <img src="ValidateImage.aspx"id="picVerifyCode" height="23px" style="cursor:pointer;border: 1px solid #333333;vertical-align:text-bottom;" onclick="UpdateImage()" title="点击更换验证码图片!"/>
			        <span id="txtCodeTip"></span>
			    </td>
		      </tr>
			</table>
			</div>
			<!--推荐人结束-->
			
			<!--服务条款开始-->
			<div class="title lan f14 bold mtop13">服务条款</div>
			<div >
			<table width="100%" border="0" cellpadding="0" cellspacing="0" class="mtop13 mbottom10">
			  <tr>
				<td width="230" align="right" class="f14">&nbsp;</td>
				<td><input id="chkAgree" checked="checked" type="checkbox" name="chkAgree" />
                我已阅读并同意 <a href="/Agreement.aspx" target="_blank">用户服务条款</a>
                <span id="chkAgreeTip"></span>
                </td>
			  </tr>
			  <tr>
				<td align="right" class="f14">&nbsp;</td>
				<td class="padding15">
				    <asp:Button ID="btnRegister" runat="server" Text="注 册" CssClass="btn1" 
                        onclick="btnRegister_Click" />&nbsp;&nbsp;
				    <input type="reset" value="重 置" class="btn1" />
				    <input name="reg" type="hidden" id="reg" value="true" />
				    
				</td>
		      </tr>
			</table>
			</div>
			<!--服务条款结束-->			
			<div class="clear"></div>
		</div>
		<div class="regBottom"><div class="clear"></div></div>
		<div class="clear"></div>
	</div>
	<div class="clear"></div>
</div>
</form>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
