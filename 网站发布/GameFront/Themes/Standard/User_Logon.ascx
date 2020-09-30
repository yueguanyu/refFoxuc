<%@ Control Language="C#" AutoEventWireup="true" %>

<script type="text/javascript">
    function UpdateImage(){
        document.getElementById("picVerifyCode").src="../../ValidateImage.aspx?r="+Math.random();
    }

    $(document).ready(function() {
        $.ajax({
            contentType: "application/json",
            url: "/WS/WSAccount.asmx/GetUserInfo",
            data: "{}",
            type: "POST",
            dataType: "json",
            success: function(json) {
                json = eval("(" + json.d + ")");

                if (json.success == 'success') {
                    var name = SubString(json.account, 12, "");
                    $("#imgFace").attr("src", "/gamepic/face" + json.fid + ".gif");
                    $("#dlInfo").html("<span>帐号：" + name + "</span><span>ＩＤ：" + json.gid + "</span>").attr("title", json.account);
                    $("#mOrder").html(json.morder);
                    $("#loves").html(json.loves);
                    $("#divLogin").hide();
                    $("#divInfo").show();
                }
            },
            error: function(err, ex) {
                alert(err.responseText);
                UpdateImage();
            }
        });
    
        $("#loginName").focus();
        $("#txtCode").keydown(function(e) {
            if (e.which == 13)
                $("#btnLogin").click();
        });
        $("#loginPass").keydown(function(e) {
            if (e.which == 13)
                $("#btnLogin").click();
        });
        $("#loginName").keydown(function(e) {
            if (e.which == 13)
                $("#btnLogin").click();
        });

        $("#btnLogin").click(function() {
            var name = $("#loginName");
            var pass = $("#loginPass");
            var code = $("#txtCode");

            if ($.trim(name.val()) == "") { name.focus(); return alert("请输入你的用户名！"); }
            if ($.trim(pass.val()) == "") { pass.focus(); return alert("请输入你的密码！"); }
            if ($.trim(code.val()) == "") { code.focus(); return alert("请输入你的验证码！"); }

            $.ajax({
                contentType: "application/json",
                url: "/WS/WSAccount.asmx/Logon",
                data: "{userName:'" + name.val() + "',userPass:'" + pass.val() + "',code:'" + code.val() + "'}",
                type: "POST",
                dataType: "json",
                success: function(json) {
                    json = eval("(" + json.d + ")");

                    if (json.success == "error") {
                        $("#divLogin").show();

                        alert(json.msg);
                        code.val("");
                        pass.val("");
                        pass.focus();
                        UpdateImage();
                    } else if (json.success == "success") {
                        $("#imgFace").attr("src", "/gamepic/face" + json.fid + ".gif");
                        var name = SubString(json.account, 12, "");
                        //$("#dlInfo").html("帐号：" + name + "<br />ＩＤ：" + json.gid + "<br />").attr("title", json.account);
                        $("#dlInfo").html("<span>帐号：" + name + "</span><span>ＩＤ：" + json.gid + "</span>").attr("title", json.account);
                        $("#mOrder").html(json.morder);
                        $("#loves").html(json.loves);
                        $("#divLogin").hide();
                        $("#divInfo").show();
                    }
                },
                error: function(err, ex) {
                    alert(err.responseText);
                    UpdateImage();
                }
            });
        })
    })
</script>

<!--会员登录开始-->
<div class="serve" id="divLogin">
	<div class="serve1 bold hui3">会员登录</div>
  	<div class="loginBg">
		<div class="login">
		<ul>
		<li>游戏帐号：<input id="loginName" type="text" name="textfield" /></li>
		<li>密　　码：<input id="loginPass" type="password" name="textfield" /></li>
		<li><span><img id="picVerifyCode" src="../../ValidateImage.aspx" width="56" height="22" onclick="UpdateImage()" alt="点击图片更换验证码" /></span>验&nbsp;&nbsp;证&nbsp;码：<input id="txtCode" type="text" name="textfield" class="wd" /></li>
		<li><label><a href="/Member/ReLogonPass.aspx" class="lh">忘记密码？</a></label>
		  <img src="/images/login.gif" id="btnLogin" alt="" border="0" class="mleft60" /></li>
		<li><a href="/Register.aspx"><img src="/images/reg.gif" alt="" width="111" height="31" border="0" class="mleft45 " /></a></li>
		</ul>
		<div class="clear"></div>
	  </div>
	  <div class="clear"></div>	
  </div>
	<div class="serve4"><div class="clear"></div></div>
</div>
<!--会员登录结束-->

<!--会员登录开始-->
<div class="serve" id="divInfo" style=" display:none;">
	<div class="serve1 bold hui3">个人信息</div>
  	<div class="loginBg">
		<!--会员登录后开始-->
		<div class="login">
			<div class="logined">
				<dl class="loginedDl2"><img src="" id="imgFace" /></dl>
				<dl class="loginedDl" id="dlInfo"></dl>
			</div>
			<div class="logined1">会员类型：</div><div class="logined2" id="mOrder"></div>
			<div class="logined1">魅 力 值：</div><div class="logined2 bold hong" id="loves"></div>
			<div class="logined3">
			    [<a href="/Member/MIndex.aspx" class="lh">个人中心</a>]&nbsp;&nbsp;&nbsp;&nbsp;
			    [<a href="/Member/ModifyLogonPass.aspx" class="lh">修改密码</a>]
			</div>
			<div class="logined4"><a href="/Index.aspx?exit=true" class="outLink" onclick="return confirm('您确定要退出吗？');">退出登录</a></div>
		<div class="clear"></div>
	  	</div>
	  <!--会员登录后结束-->
	  <div class="clear"></div>	
  </div>
	<div class="serve4"><div class="clear"></div></div>
</div>
<!--会员登录结束-->
