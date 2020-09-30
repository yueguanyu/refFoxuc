<%@ Control Language="C#" AutoEventWireup="true" CodeBehind="Index_Service.ascx.cs" Inherits="Game.Web.Themes.Standard.Index_Service" %>
<%@ Import Namespace="Game.Facade" %>

<!--服务中心开始-->
<div class="serve mtop8">
  <div class="serve1 bold"><div class="bold hui3 left">服务中心</div><span><a href="/Member/MIndex.aspx" class="lh6">更多</a></span></div>
	<div class="serveBg">
		<div class="left mbottom7">
		<div class="serve2">帐号</div>
		<div class="serve3">
			<ul>
			<li><a href="/Member/ModifyNikeName.aspx" class="lw" >修改昵称</a></li>
			<li><a href="/Member/ModifyUserInfo.aspx" class="lw" >修改资料</a></li>
			<li><a href="/Member/ModifyLogonPass.aspx" class="lw" >修改密码</a></li>
			</ul>
		</div>
		</div>
		
		<div class="left mbottom7">
		<div class="serve2">安全</div>
		<div class="serve3">
			<ul>
			<li><a href="/Member/ApplyProtect.aspx" class="lw" >申请密码保护</a></li>
			<li><a href="/Member/ModifyProtect.aspx" class="lw" >修改密码保护</a></li>
			</ul>
		</div>
		</div>
		
		<div class="left mbottom7">
		<div class="serve2">银行</div>
		<div class="serve3">
			<ul>
			<li><a href="/Member/InsureIn.aspx" class="lw" >存款</a></li>
			<li><a href="/Member/InsureOut.aspx" class="lw" >取款</a></li>
			<li><a href="/Member/InsureTransfer.aspx" class="lw" >转账</a></li>
			<li><a href="/Member/ModifyInsurePass.aspx" class="lw" >修改银行密码</a></li>
			</ul>
		</div>
		</div>
		
		<div class="left mbottom7">
		<div class="serve2">会员</div>
		<div class="serve3">
			<ul>
			<li><a href="/Member/ClearScore.aspx" class="lw" >负分清零</a></li>
			<li><a href="/Member/ClearFlee.aspx" class="lw" >逃跑清零</a></li>
			<li><a href="/Member/MoorMachine.aspx" class="lw" >固定机器</a></li>
			</ul>
		</div>
		</div>
		<div class="left">
		<div class="serve2">客服</div>
		<div class="serve3">
			<ul>
			<li><a href="/Service/Customer.aspx" class="lw" >联系客服</a></li>
			<li><a href="/Service/IssueList.aspx" class="lw" >常见问题</a></li>
			<li><a href="/Service/FeedbackList.aspx" class="lw" >问题反馈</a></li>
			</ul>
		</div>
		</div>
	<div class="clear"></div>
	</div>
	<div class="serve4"><div class="clear"></div></div>
</div>
<!--服务中心结束-->
