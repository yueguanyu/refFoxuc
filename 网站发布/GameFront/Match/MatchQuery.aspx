<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="MatchQuery.aspx.cs" Inherits="Game.Web.Match.MatchQuery" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id=Head1 runat="server">
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
				<li><a href="/Match/MatchOrder.aspx?XID=<%= matchID %>">比赛排名</a></li>
				<li class="current"><a href="/Match/MatchQuery.aspx?XID=<%= matchID %>">个人成绩</a></li>
				</ul>
		  	</div>
			<div class="right"></div>
		</div>
	</div>
	
    <div class="contMain">
	<div class="top3"></div>
    <div class="mainBox">
      <div class="mainBox1 f12">
	      
          <asp:Panel ID=plData runat="server">
           <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#C5E7FD">
            <tr align="center" class="bold">
                <td height="35" class="leftTop trBg">场次</td>
                <td class="leftTop trBg">成绩</td>
                <td class="leftTop trBg">排名</td>
                <td class="leftTop trBg">时间</td>                     
            </tr>   
             <asp:Repeater ID="rptRank" runat="server">
               <ItemTemplate>
                     <tr align="center" class="trBg2">
                        <td height="35"><%# Eval("MatchNo").ToString()%></td>
                        <td><%# Eval( "MatchScore" ).ToString()%></td>
                        <td><%# Eval( "Rank" ).ToString( ) == "0" ? "放弃比赛" : Eval( "Rank" ).ToString( )%></td>
                        <td><%# Eval( "RecordDate" ).ToString( )%></td>
                    </tr>
               </ItemTemplate>
               <AlternatingItemTemplate>
                     <tr align="center" class="trBg2">
                        <td height="35"><%# Eval("MatchNo").ToString()%></td>
                        <td><%# Eval( "MatchScore" ).ToString()%></td>
                        <td><%# Eval( "Rank" ).ToString( ) == "0" ? "放弃比赛" : Eval( "Rank" ).ToString( )%></td>
                        <td><%# Eval( "RecordDate" ).ToString( )%></td>
                    </tr>
               </AlternatingItemTemplate>        
             </asp:Repeater>
           </table>
           <form id="form1" name="form1" runat="server">
            <div class="mtop13" style="text-align:right;">
                  <webdiyer:AspNetPager ID="anpPage"  runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页" PageSize="20" NextPageText="下页                        " PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Never"
                        LayoutType="Table" NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount% 每页：%PageSize%"
                       UrlPaging="false" onpagechanging="anpPage_PageChanging">
                  </webdiyer:AspNetPager>
            </div>
          </form>    
          </asp:Panel>
           <asp:Panel ID="plMessage" runat="server" Visible="false">
            抱歉！您没有取得比赛排名
          </asp:Panel>
          <br />
          <div class="title2 lan bold">温馨提示</div>
            <br />
            <span class="hui3">1.系统只提供当天的个人比赛排名信息。</span>   <br /> <br />
            <span class="hui3">2.有任何问题，请您联系客服中心。</span>
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

