<%@ Control Language="C#" AutoEventWireup="true"  %>

<script type="text/javascript">
    $(document).ready(function(){            
        $.ajax({
            contentType: "application/json",
            url: "/WS/WSAccount.asmx/GetUserInfo",
            data: "{}",
            type:"POST",
            dataType: "json",
            success:function(json){
                json = eval("("+json.d+")");
                
                if(json.success=='success'){
                    $("#imgFace").attr("src","/gamepic/face"+json.fid+".gif");
                    var c = judgeString(json.account);
                    var name = SubString(json.account,12,"");
                    $("#dlInfo").html("<span>帐号：" + name + "</span><span>ＩＤ：" + json.gid + "</span>").attr("title", json.account);
                    $("#mOrder").html(json.morder);
                    $("#loves").html(json.loves);
                }
            },
            error:function(err,ex){
                alert(err.responseText);
                UpdateImage();
            }
        });
    })
    
    function judgeString(str){
        var c = 0;
        var len = str.length;
        for(var i=0;i<len;i++){
            var txt = str.charCodeAt(i);
            if(txt>128)
                c++;
        }
        return c;
    }
</script>

<!--会员登录开始-->
<div class="serve">
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
