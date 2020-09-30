<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GameChart.aspx.cs" Inherits="Game.Web.GameChart" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="GameSidebar" Src="~/Themes/Standard/Game_Rank.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id=Head1 runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/game_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/cz_layout.css" />
</head>
<body>
<qp:Header ID="sHeader" runat="server" />
<!--页面主体开始-->
<div class="main" style="margin-top:8px;">
	<!--左边部分开始-->
	<div class="mainLeft1">
<div class="LeftSide">
	<!--游戏列表开始-->
	<div class="gameList"><div class="clear"> </div></div>
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
	    <!--游戏介绍内容开始-->		
		<div class="recharge">		
		    <div class="gameTitle bold hui3">玩家排行</div>
			<div class="gameTop1">			
			  <table width="100%" border="0">
                <tr>
                  <td width="9"><img src="/images/arr.gif" width="9" height="9" /></td>
                  <td width="668" style="text-align:left; font-weight:bold;">
                    财富游戏
                  </td>
                </tr>                
              </table>
		      <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class="leftTop">
                <tr align="center" class="bold">
                  <td class="titleBg" style="width:10%;">名次</td>
                  <td class="titleBg" style="width:30%;">昵称</td>
                  <td class="titleBg" style="width:14%;">ID</td>
                  <td class="titleBg" style="width:30%;">分数</td>
                  <td class="titleBg" style="width:8%;">赢局</td>
                  <td class="titleBg" style="width:8%;">输局</td>
                </tr>
                  <asp:Repeater ID="rptRank" runat="server">
                    <ItemTemplate>
                      <tr align="center" class="trBg1">
                          <td height="35"><%# number++%></td>
                          <td><%#  Game.Utils.TextUtility.CutStringProlongSymbol(GetNickNameByUserID( Convert.ToInt32(Eval( "UserID" ) ) ),25 )%></td>
                          <td><%# GetGameIDByUserID( Convert.ToInt32( Eval( "UserID" ) ) )%></td>
                          <td><%# Eval("Score") %></td>
                          <td><%# Eval( "WinCount" )%></td>
                          <td><%# Eval( "LostCount" ) %></td>
                        </tr>
                    </ItemTemplate>
                  </asp:Repeater>         
              </table>
			<div class="clear"></div>
	 	</div>	 	
		<div class="clear"></div>
		<div class="recharge2"><div class="clear"></div></div>
		<div class="clear"></div>
		</div>			
		<!--游戏介绍内容结束-->	
	<div class="clear"></div>
	</div>
	<!--右边部分结束-->		
<div class="clear"></div>
</div>
<!--页面主体结束-->
<qp:Footer ID="sFooter" runat="server" />
</body>
</html>

