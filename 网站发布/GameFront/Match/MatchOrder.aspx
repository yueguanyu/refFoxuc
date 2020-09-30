<%@ OutputCache Duration="600" VaryByParam="XID" %>
<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="MatchOrder.aspx.cs" Inherits="Game.Web.Match.MatchOrder" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/contest_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/user_layout.css" /> 
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main" style="margin-top:8px;">
	<div class="contTop">
		<div class="nav">
			<div class="left"></div>
			<div class="center">
				<ul>
				<li><a href="/Match/MatchView.aspx?XID=<%= matchID %>">赛事公告</a></li>
				<li class="current"><a href="/Match/MatchOrder.aspx?XID=<%= matchID %>">比赛排名</a></li>
				<li><a href="/Match/MatchQuery.aspx?XID=<%= matchID %>">个人成绩</a></li>
				</ul>
		  	</div>
			<div class="right"></div>
		</div>
	</div>
	
    <div class="contMain">
	<div class="top2"></div>
    <div class="mainBox">
      <div class="mainBox1 f12">
        <asp:Panel ID="plData" runat="server">
         <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#C5E7FD">
            <tr align="center" class="bold">
                <td height="35" class="leftTop trBg">排名</td>
                <td class="leftTop trBg">昵称</td>
                <td class="leftTop trBg">游戏ＩＤ</td>
                <td class="leftTop trBg">成绩</td>                     
            </tr>   
             <asp:Repeater ID="rptRank" runat="server">
               <ItemTemplate>
                     <tr align="center" class="trBg2">
                        <td height="35"><%# number++%></td>
                        <td><%# GetNickNameByUserID( Convert.ToInt32( Eval( "UserID" ) ) )%></td>
                        <td><%# GetGameIDByUserID( Convert.ToInt32(Eval( "UserID" )))%></td>
                        <td><%# Eval( "Score" ).ToString( )%></td>
                    </tr>
               </ItemTemplate>
               <AlternatingItemTemplate>
                        <tr align="center" class="trBg1">
                        <td height="35"><%# number++%></td>
                        <td><%# GetNickNameByUserID( Convert.ToInt32( Eval( "UserID" ) ) )%></td>
                        <td><%# GetGameIDByUserID( Convert.ToInt32(Eval( "UserID" )))%></td>
                        <td><%# Eval( "Score" ).ToString( )%></td>
                    </tr>
               </AlternatingItemTemplate>        
             </asp:Repeater>
           </table>
          </asp:Panel>
          <asp:Panel ID="plMessage" runat="server" Visible="false">
            暂无比赛排名记录
          </asp:Panel>
      </div>
  	</div>
	<div class="bottom"></div>
</div>
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
