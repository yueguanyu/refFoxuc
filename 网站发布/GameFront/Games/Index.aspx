<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Index.aspx.cs" Inherits="Game.Web.Games.Index" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="GameSidebar" Src="~/Themes/Standard/Game_Sidebar.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/game_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/cz_layout.css" />
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<form id="form1" runat="server">

<!--页面主体开始-->
<div class="main" style="margin-top:8px;">
	<!--左边部分开始-->
	<div class="mainLeft1">
	
<div class="LeftSide">
	<!--游戏列表开始-->
	<div class="gameList"><div class="clear"></div></div>
	<div class="gameListBg">
	    
	 <qp:GameSidebar ID="sGameSidebar" runat="server" />
	    		
	</div>
	<div class="gameListEnd"></div>
	<!--游戏列表结束-->
</div>
<div class="clear"></div>
	</div>
	<!--左边部分结束-->
	
	<!--右边部分开始-->
	<div class="mainRight1">
	    
		<!--Banner下载条开始-->
	    <div class="gameBanner"><img src="/images/gameBanner.jpg" border="0" usemap="#Map" />
        <map name="Map" id="Map">
          <area shape="rect" coords="515,150,692,193" href="/Download/WHGameFull.exe" />
        </map>
        </div>
		<!--Banner下载条结束-->
		
		<!--下载开始-->
		<div class="recharge mtop10">		
			<div class="gameDownL left">
				<div class="title"></div>
				<div class="Bg">
					<div class="left">
						<div class="down"></div>
						<div><a href="/Download/Plaza.exe" class="down1"  hidefocus="true"></a></div>
					</div>
					<div class="right">软件名称：游戏大厅精简版<br />发布日期：2011年12月18日<br />版 本 号：6.6.0.3<br />操作系统：Windows 2000/XP/vista/7<br />软件大小：4.42MB</div>
				<div class="clear"></div>
				</div>
			<div class="clear"></div>
			</div>	
			<div class="gameDownL right">
				<div class="title1"></div>
				<div class="Bg">
					<div class="left">
						<div class="down2"></div>
						<div><a href="/Download/WHGameFull.exe" class="down1"  hidefocus="true"></a></div>
					</div>
					<div class="right">软件名称：游戏大厅完整版<br />
					  发布日期：2011年12月18日<br />版 本 号：6.6.0.3<br />操作系统：Windows 2000/XP/vista/7<br />软件大小：49.88MB</div>
				<div class="clear"></div>
				</div>
			<div class="clear"></div>
			</div>	
		<div class="clear"></div>
		</div>
		<!--下载结束-->

		<!--最新游戏开始-->
		<div class="recharge mtop10">
			<div class="gameTitle bold hui3">最新游戏 <img src="/images/new.gif" width="24" height="11" /></div>
			<div class="gameNew">
			  <ul>
				<li>
				<span><img src="/img/g1.gif" /></span>
				<label><a href="#" target="_blank" class="lh">掼蛋</a></label>
				<label><a href="/GameRules.aspx?KindID=3"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="/Download.aspx?KindID=3"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g2.gif" /></span>
				<label><a href="#" target="_blank" class="lh">广东麻将</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g3.gif" /></span>
				<label><a href="#" target="_blank" class="lh">拖拉机</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g4.gif" /></span>
				<label><a href="#" target="_blank" class="lh">斗地主</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="/Download.aspx?KindID=2"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g5.gif" /></span>
				<label><a href="#" target="_blank" class="lh">中国象棋</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g6.gif" /></span>
				<label><a href="#" target="_blank" class="lh">港式五张</a></label>
				<label><a href="/GameRules.aspx?KindID=1"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="/Download.aspx??KindID=1"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
			  </ul>
			<div class="clear"></div>
		 	</div>
			<div class="clear"></div>
			<div class="recharge2"><div class="clear"></div></div>
		<div class="clear"></div>
		</div>
		<!--最新游戏结束-->
		
		<!--热门游戏开始-->
		<div class="clear"></div>
		<div class="recharge mtop10">
			<div class="gameTitle bold hui3">热门游戏 <img src="/images/hot.gif" width="17" height="8" /></div>
			<div class="gameNew">
			  <ul>
				<li>
				<span><img src="/img/g1.gif" /></span>
				<label><a href="#" target="_blank" class="lh">掼蛋</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g2.gif" /></span>
				<label><a href="#" target="_blank" class="lh">广东麻将</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g3.gif" /></span>
				<label><a href="#" target="_blank" class="lh">拖拉机</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g4.gif" /></span>
				<label><a href="#" target="_blank" class="lh">斗地主</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g5.gif" /></span>
				<label><a href="#" target="_blank" class="lh">中国象棋</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g6.gif" /></span>
				<label><a href="#" target="_blank" class="lh">港式五张</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g1.gif" /></span>
				<label><a href="#" target="_blank" class="lh">掼蛋</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g2.gif" /></span>
				<label><a href="#" target="_blank" class="lh">广东麻将</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g3.gif" /></span>
				<label><a href="#" target="_blank" class="lh">拖拉机</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g4.gif" /></span>
				<label><a href="#" target="_blank" class="lh">斗地主</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g5.gif" /></span>
				<label><a href="#" target="_blank" class="lh">中国象棋</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
				<li>
				<span><img src="/img/g6.gif" /></span>				
				<label><a href="#" target="_blank" class="lh">港式五张</a></label>
				<label><a href="#" target="_blank"><img src="/images/js.gif" width="33" height="18" border="0" /></a> <a href="#"><img src="/images/down.gif" width="33" height="18" border="0" /></a></label>
				</li>
			  </ul>
			<div class="clear"></div>
		 	</div>
			<div class="clear"></div>
			<div class="recharge2"><div class="clear"></div></div>
		<div class="clear"></div>
		</div>
		<!--热门游戏结束-->
	<div class="clear"></div>
	</div>
	<!--右边部分结束-->
<div class="clear"></div>
</div>
<!--页面主体结束-->
</form>

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
