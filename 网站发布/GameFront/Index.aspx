<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Index.aspx.cs" Inherits="Game.Web.Index" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Links" Src="~/Themes/Standard/Index_Links.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>
<%@ Register TagPrefix="qp" TagName="Logon" Src="~/Themes/Standard/User_Logon.ascx" %>
<%@ Register TagPrefix="qp" TagName="Info" Src="~/Themes/Standard/User_Info.ascx" %>
<%@ Register TagPrefix="qp" TagName="Advice" Src="~/Themes/Standard/Common_Advice.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="IndexService" Src="~/Themes/Standard/Index_Service.ascx" %>
<%@ Register TagPrefix="qp" TagName="Ranking" Src="~/Themes/Standard/Ranking.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
<link rel="Stylesheet" type="text/css" href="style/layout.css" />
<link rel="stylesheet" type="text/css" href="style/global.css" />

<script src="js/jquery-1.5.2.min.js" type="text/javascript"></script>
<script src="JS/MSClass.js" type="text/javascript"></script>
<script type="text/javascript">
    //图片按比例缩放
    var flag = false;
    function DrawImage(ImgD, iwidth, iheight) {
        //参数(图片,允许的宽度,允许的高度)
        var image = new Image();
        image.src = ImgD.src;
        if (image.width > 0 && image.height > 0) {
            flag = true;
            if (image.width / image.height >= iwidth / iheight) {
                if (image.width > iwidth) {
                    ImgD.width = iwidth;
                    ImgD.height = (image.height * iwidth) / image.width;
                } else {
                    ImgD.width = image.width;
                    ImgD.height = image.height;
                }
                ImgD.alt = image.width + "×" + image.height;
            }
            else {
                if (image.height > iheight) {
                    ImgD.height = iheight;
                    ImgD.width = (image.width * iheight) / image.height;
                } else {
                    ImgD.width = image.width;
                    ImgD.height = image.height;
                }
                ImgD.alt = image.width + "×" + image.height;
            }
        }
    } 
</script>
</head>
<body>
    <qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main mtop8">
	<!--左边部分开始-->
	<div class="mainLeft">
		<div class="LeftSide">
			<qp:Btn ID="sBtn" runat="server" />
			
			<qp:IndexService ID="sIndexService" runat="server" />
			
			<qp:Question ID="sQuestion" runat="server" />
			
			<qp:Advice ID="sAdvice" runat="server" />
			
		<div class="clear"></div>
		</div>
		<div class="RightSide">
			<!--banner开始-->
			<div align="center">
                <script type="text/javascript" src="js/flash.js"></script>
            </div>

			<!--banner结束-->
			<!--新闻公告开始-->
			<div class="news mtop10">
			  <div class="news1"><div class="bold hui3 left">新闻公告</div><span><a href="/News/NewsList.aspx" target="_self" class="lh6">更多</a></span></div>
				<div class="newsBg">
					<ul>
                        <asp:Repeater ID="rptNews" runat="server">
                            <ItemTemplate>
                                <li><span class="hui6"><%# Eval("IssueDate","{0:yyyy-MM-dd}")%></span>[<%# GetNewsType(Eval("ClassID")) %>]&nbsp;<a href='/News/NewsView.aspx?XID=<%# Eval("NewsID") %>' class="lh" target="_blank"><%# Eval("Subject")%></a></li>
                            </ItemTemplate>
                        </asp:Repeater>
					</ul>
				</div>
			    <div class="clear"></div>
			</div>
			<!--新闻公告结束-->
			
			<!--游戏商城开始-->
			<div class="clear"></div>
			<div class="news mtop10">
				<div class="news1"><div class="bold hui3 left">游戏商城</div><span><a href="/Shop/ShopIndex.aspx" class="lh6">更多</a></span></div>
				<div class="shopBg">
					<div class="photoScLeft"><img src="images/SCleft.gif"  id="left2" alt="向左滚动"/></div>
					<div id="marqueedivcontrol1">
					<div class="shopBg1">
						<ul>
						<li><span><img src="img/cd.gif" width="70" height="70" /></span><span><a href="/Shop/ShopIndex.aspx" class="lh" >臭蛋</a></span><span class="cheng">1000金币</span><span><a href="/Shop/ShopIndex.aspx"><img src="images/viewInfo.gif" border="0"/></a></span></li>
						<li><span><img src="img/xy.gif" width="70" height="70"/></span><span><a href="/Shop/ShopIndex.aspx" class="lh" >香烟</a></span><span class="cheng">1000金币</span><span><a href="/Shop/ShopIndex.aspx"><img src="images/viewInfo.gif" border="0"/></a></span></li>
						<li><span><img src="img/qc.gif" width="70" height="70"/></span><span><a href="/Shop/ShopIndex.aspx" class="lh" >汽车</a></span><span class="cheng">1000金币</span><span><a href="/Shop/ShopIndex.aspx"><img src="images/viewInfo.gif" border="0"/></a></span></li>
						<li><span><img src="img/zd.gif" width="70" height="70"/></span><span><a href="/Shop/ShopIndex.aspx" class="lh" >炸弹</a></span><span class="cheng">1000金币</span><span><a href="/Shop/ShopIndex.aspx"><img src="images/viewInfo.gif" border="0"/></a></span></li>
						<li><span><img src="img/pj.gif" width="70" height="70"/></span><span><a href="/Shop/ShopIndex.aspx" class="lh" >啤酒</a></span><span class="cheng">1000金币</span><span><a href="/Shop/ShopIndex.aspx"><img src="images/viewInfo.gif" border="0"/></a></span></li>
						</ul>
						<div class="clear"></div>
					</div><div class="clear"></div>
					</div>
					<div class="photoScRight"><img src="images/SCright.gif"  id="right2" alt="向右滚动"/></div>
			  </div>
			</div>
			<!--游戏商城结束-->
			
			<!--热门游戏开始-->
			<div class="clear"></div>
			<div class="news mtop10">
			  <div class="news1"><div class="bold hui3 left">热门游戏</div>
				<span><a href="/Games/Index.aspx" target="_self" class="lh6">更多</a></span></div>
				<div class="clear"></div>
				<div class="gameHotBg">
					<ul>
					<li><span><a href="/Games/Index.aspx"><img src="img/h1.gif" border="0" /></a><a href="/Games/Index.aspx" class="lh">掼蛋</a></span></li>
					<li><span><a href="/Games/Index.aspx"><img src="img/h2.gif" border="0" /></a><a href="/Games/Index.aspx" class="lh">斗地主</a></span></li>
					<li><span><a href="/Games/Index.aspx"><img src="img/h3.gif" border="0" /></a><a href="/Games/Index.aspx"class="lh">港式五张</a></span></li>
					<li><span><a href="/Games/Index.aspx"><img src="img/h4.gif" border="0" /></a><a href="/Games/Index.aspx" class="lh">中国象棋</a></span></li>
					</ul>				
				</div>
			</div>
			<!--热门游戏结束-->
			
			<!--玩家靓照开始-->
			<div class="clear"></div>
			<div class="news mtop10">
				<div class="news1 bold hui3 ">玩家靓照</div>
				<div class="photoBg">
				  	<ul>
					<li><img src="img/p1.gif"></li>
					<li><img src="img/p2.gif"></li>
					<li><img src="img/p3.gif"></li>
					<li><img src="img/p4.gif"></li>
					</ul>
				</div>
			</div>
			<!--玩家靓照结束-->
			
		<div class="clear"></div>
		</div>
	<div class="clear"></div>
	</div>
	
	
	<!--左边部分结束-->
	
	<div class="mainRight">
		    <% if (Fetch.GetUserCookie() == null)
               { %>
            <qp:Logon ID="sLogon" runat="server" />
            <%}
               else
               { %>
            <qp:Info ID="sInfo" runat="server" />
            <% } %>
	
			<!--玩家排行榜开始-->
			<qp:Ranking ID="Ranking2" runat="server" />
			<!--玩家排行榜结束-->
	
			<qp:Service ID="sService" runat="server" />
	
	</div>
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Links ID="sLinks" runat="server" />

<qp:Footer ID="sFooter" runat="server" />

<script type="text/javascript">
    var marquee3 = new Marquee("marqueedivcontrol1");
    var left2 = document.getElementById("left2");
    var right2 = document.getElementById("right2");
    marquee3.Direction = "left";
    marquee3.Step = 1;
    marquee3.Width = 420;
    marquee3.Height =134;
    marquee3.Timer = 20;
    marquee3.ScrollStep = 1; //此句禁止鼠标控制
    marquee3.Start();
    left2.onmouseover = function() { marquee3.Direction = 2 }
    left2.onmouseout = left2.onmouseup = function() { marquee3.Step = marquee3.BakStep }
    left2.onmousedown = right2.onmousedown = function() { marquee3.Step = marquee3.BakStep + 2 }
    right2.onmouseover = function() { marquee3.Direction = 3 }
    right2.onmouseout = right2.onmouseup = function() { marquee3.Step = marquee3.BakStep }
</script>
</body>
</html>
