<%@ Control Language="C#" AutoEventWireup="true" CodeBehind="User_Sidebar.ascx.cs" Inherits="Game.Web.Themes.Standard.User_Sidebar" %>
<%@ Import Namespace="Game.Facade" %>

<!--用户菜单开始-->
<div class="userNavi"><div class="clear"></div></div>
<div class="userNaviBg">
	<!--帐号服务开始-->
	<div class="userNaviTitle1" id="S_1" onclick="JavaScript:showHide(document.getElementById('M_1'));"><label class="userType1">账号服务</label></div>			
	<div class="clear"></div>
	<ul id="M_1">
	<li><span class="userErr1"><a href="/Member/ModifyNikeName.aspx" class="lh">修改昵称</a></span></li>
	<li><span class="userErr1"><a href="/Member/ModifyUserInfo.aspx" class="lh">修改资料</a></span></li>
	<li><span class="userErr1"><a href="/Member/ModifyFace.aspx" class="lh">修改头像</a></span></li>
	<li><span class="userErr1"><a href="/Member/ModifyLogonPass.aspx" class="lh">修改登录密码</a></span></li>
	<li><span class="userErr1"><a href="/Member/ModifyInsurePass.aspx" class="lh">修改保险柜密码</a></span></li>
	</ul>
	<!--帐号服务结束-->
	
	<!--帐号安全开始-->
	<div class="userNaviTitle1" id="S_2" onclick="JavaScript:showHide(document.getElementById('M_2'));"><label class="userType2">账号安全</label></div>				
	<div class="clear"></div>
	<ul id="M_2">
	<li><span class="userErr1"><a href="/Member/ApplyProtect.aspx" class="lh">申请密码保护</a></span></li>
	<li><span class="userErr1"><a href="/Member/ModifyProtect.aspx" class="lh">修改密码保护</a></span></li>
	<li><span class="userErr1"><a href="/Member/ReLogonPass.aspx" class="lh">找回登录密码</a></span></li>
	<li><span class="userErr1"><a href="/Member/ReInsurePass.aspx" class="lh">找回保险柜密码</a></span></li>
	<li><span class="userErr1"><a href="/Member/ApplyPasswordCard.aspx" class="lh">申请密保卡</a></span></li>
	<li><span class="userErr1"><a href="/Member/ExitPasswordCard.aspx" class="lh">取消密保卡</a></span></li>
	</ul>
	<!--帐号安全结束-->
	
	<!--保险柜服务开始-->				
	<div class="userNaviTitle1" id="S_3" onclick="JavaScript:showHide(document.getElementById('M_3'));"><label class="userType3">保险柜服务</label></div>
	<div class="clear"></div>
	<ul id="M_3">
	<li><span class="userErr1"><a href="/Member/InsureIn.aspx" class="lh">存款</a></span></li>
	<li><span class="userErr1"><a href="/Member/InsureOut.aspx" class="lh">取款</a></span></li>
	<li><span class="userErr1"><a href="/Member/InsureTransfer.aspx" class="lh">转帐</a></span></li>
	<li><span class="userErr1"><a href="/Member/InsureRecord.aspx" class="lh">交易明细</a></span></li>
	<li><span class="userErr1"><a href="/Member/ConvertPresent.aspx" class="lh">魅力兑换</a></span></li>
	<li><span class="userErr1"><a href="/Member/ConvertRecord.aspx" class="lh">魅力兑换记录</a></span></li>
    <li><span class="userErr1"><a href="/Member/ConvertMedal.aspx" class="lh">奖牌兑换</a></span></li>
	<li><span class="userErr1"><a href="/Member/ConvertMedalRecord.aspx" class="lh">奖牌兑换记录</a></span></li>
	</ul>
	<!--保险柜服务结束-->
	
	<!--充值中心开始-->	
	<div class="userNaviTitle1" id="S_4" onclick="JavaScript:showHide(document.getElementById('M_4'));"><label class="userType4">充值中心</label></div>
	<div class="clear"></div>
	<ul id="M_4">
	<li><span class="userErr1"><a href="/Pay/PayIndex.aspx" class="lh">会员介绍</a></span></li>
	<li><span class="userErr1"><a href="/Member/PayRecord.aspx" class="lh">充值记录</a></span></li>
	</ul>
	<!--充值中心结束-->	
		
	<!--会员服务开始-->	
	<div class="userNaviTitle1" id="S_5" onclick="JavaScript:showHide(document.getElementById('M_5'));"><label class="userType4">会员服务</label></div>
	<div class="clear"></div>
	<ul id="M_5">
	<li><span class="userErr1"><a href="/Member/ClearScore.aspx" class="lh">负分清零</a></span></li>
	<li><span class="userErr1"><a href="/Member/ClearFlee.aspx" class="lh">逃跑清零</a></span></li>
	<li><span class="userErr1"><a href="/Member/GameRecord.aspx" class="lh">游戏记录</a></span></li>
	<li><span class="userErr1"><a href="/Member/MoorMachine.aspx" class="lh">固定机器</a></span></li>
	</ul>
	<!--会员服务结束-->	
	
	<!--推广中心开始-->	
	<div class="userNaviTitle1" id="S_6" onclick="JavaScript:showHide(document.getElementById('M_6'));"><label class="userType4">推广服务</label></div>
	<div class="clear"></div>
	<ul id="M_6">
	<li><span class="userErr1"><a href="/Member/SpreadIn.aspx" class="lh">业绩查询</a></span></li>
	<li><span class="userErr1"><a href="/Member/SpreadBalance.aspx" class="lh">业绩结算</a></span></li>
	<li><span class="userErr1"><a href="/Member/SpreadInfo.aspx" class="lh">推广明细查询</a></span></li>
	<li><span class="userErr1"><a href="/Member/SpreadOut.aspx" class="lh">结算明细查询</a></span></li>
	</ul>
	<!--推广中心结束-->
</div>
<div class="gameListEnd"></div>
<!--用户菜单结束-->


<script type="text/javascript">    
    function setCookie2(sName, sValue) {
        var expires = new Date();
        expires.setTime(expires.getTime() + 16 * 60 * 1000);
        document.cookie = sName + "=" + escape(sValue) + "; expires=" + expires.toGMTString() + "; path=/";
    }
    
    function getCookie (sName) {
	    var aCookie = document.cookie.split("; ");
	    for (var i=0; i < aCookie.length; i++) {
		    var aCrumb = aCookie[i].split("=");
		    if (sName == aCrumb[0])
			    return unescape(aCrumb[1]);
	    }
	    return null;
    }
    
    function showHide(obj) {
        var oStyle = obj.style;
        var imgId = obj.id.replace("M","S");
        
        if(oStyle.display == "none")
        {
            $(obj).slideDown("fast");
            document.getElementById(imgId).className = "userNaviTitle1";
            setCookie2(obj.id,"on")
        }
        else
        {
            $(obj).slideUp("normal");
            document.getElementById(imgId).className = "userNaviTitle2";
            setCookie2(obj.id,"off")
        }
    }    
    window.onload = function() {
        for(var i=1;i<5;i++)
        {
            if (getCookie("M_"+i) == null || getCookie("M_"+i) == undefined || getCookie("M_"+i) == "on") 
            {
                document.getElementById("M_"+i).style.display = "block";
                document.getElementById("S_"+i).className = "userNaviTitle1";
            }
            else
            {
                document.getElementById("M_"+i).style.display = "none";
                document.getElementById("S_" + i).className = "userNaviTitle2";
            }
        }
    }
</script>
