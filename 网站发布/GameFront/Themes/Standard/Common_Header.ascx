<%@ Control Language="C#" AutoEventWireup="true" CodeBehind="Common_Header.ascx.cs" Inherits="Game.Web.Themes.Standard.Common_Header" %>
<%@ Import Namespace="Game.Facade" %>

<!--头部开始-->
<div class="main">
	<div class="top">
		<div class="left"><img src="/images/logo.gif"/></div>
		<div class="right mtop13 lan">
			<ul>
		    <li class="top1"><a id="lnkuchomePage" href="javascript:void(0);" onclick="setHomepage()" class="ll" >设为首页</a></li>
		    <li class="top2"><a href="/Sitemap.aspx" class="ll">网站地图</a></li>
		    <li class="top3"><a href="javascript:void(0);" onclick="addFavorites('<%= title %>> - 游戏融入生活，快乐无处不在')" class="ll">加入收藏</a></li>
		    </ul>
			<div class="clear"></div>
		</div>
	</div>
	<div class="nav">
		<div class="navLeft"></div>
		<div class="navCenter">
		<ul>
	    <li class="<%= title=="首页"?"current":"" %>"><a href="/Index.aspx" hidefocus="true">首页</a></li>
	    <li class="<%= title=="新闻公告"?"current":"" %>"><a href="/News/NewsList.aspx" hidefocus="true">新闻公告</a></li>
	    <li class="<%= title=="会员中心"?"current":"" %>"><a href="/Member/MIndex.aspx" hidefocus="true">会员中心</a></li>
	    <li class="<%= title=="充值中心"?"current":"" %>"><a href="/Pay/PayIndex.aspx" hidefocus="true">充值中心</a></li>
	    <li class="<%= title=="比赛中心"?"current":"" %>"><a href="/Match/Index.aspx" hidefocus="true">比赛中心</a></li>
	    <li class="<%= title=="游戏商场"?"current":"" %>"><a href="/Shop/ShopIndex.aspx" hidefocus="true">游戏商场</a></li>
	    <li class="<%= title=="推广系统"?"current":"" %>"><a href="/Spread/SpreadIndex.aspx" hidefocus="true">推广系统</a></li>
	    <li class="<%= title=="客服中心"?"current":"" %>"><a href="/Service/Index.aspx" hidefocus="true">客服中心</a></li>
	    </ul>	
		</div>
		<div class="navRight"></div>
  </div>
</div>
<div class="clear"></div>
<!--头部结束-->

<script type="text/javascript">
    //收藏网站
    function addFavorites(pageName) {
        var pageUrl = location.href;
        if (document.all) {
            window.external.addFavorite(pageUrl, pageName);
        }
        else if (window.sidebar) {
            window.sidebar.addPanel(pageName, pageUrl, "");
        }
}

//设为主页
function setHomepage() {
    if (document.all) {
        document.body.style.behavior = 'url(#default#homepage)';
        document.body.setHomePage(location.href);
    }
    else if (window.sidebar) {
        if (window.netscape) {
            try {
                netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
            }
            catch (e) {
                alert("该操作被浏览器拒绝，如果想启用该功能，请在地址栏内输入 about:config,然后将项 signed.applets.codebase_principal_support 值改为true");
            }
        }
        var prefs = Components.classes['@mozilla.org/preferences-service;1'].getService(Components.interfaces.nsIPrefBranch);
        prefs.setCharPref('browser.startup.homepage', location.href);
    }
}
</script>

