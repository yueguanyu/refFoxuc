<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="NewsList.aspx.cs" Inherits="Game.Web.News.NewsList" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/news_layout.css" />
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main mtop10">
	<!--左边部分开始-->
<div class="mainLeft1">
	<div class="LeftSide">		
		<qp:Btn ID="sBtn" runat="server" />
		
		<qp:Question ID="sQuestion" runat="server" />
		
		<qp:Service ID="sService" runat="server" />
		
	<div class="clear"></div>
	</div>
	<div class="clear"></div>
</div>
<!--左边部分结束-->
	
	<!--右边部分开始-->
	<div class="mainRight1">
		<!--新闻公告开始-->
		<form id="form1" runat="server">
		<div class="newsBody">
			<div class="newsTitle bold hui3">新闻公告</div>
			
			<div class="newsBg2" style=" height:auto;">
			    <ul>
                    <asp:Repeater ID="rptNewsList" runat="server" >
                        <ItemTemplate>
                            <li><span><%# Eval("IssueDate","{0:yyyy-MM-dd}")%></span><label></label>[<%# Convert.ToInt32(Eval("ClassID")) == 1 ? "新闻" : "公告"%>]&nbsp;<a href='NewsView.aspx?XID=<%# Eval("NewsID") %>' class="lh" target="_blank"><%# Eval("Subject")%></a></li>
                        </ItemTemplate>
                    </asp:Repeater>
			    </ul>  
			</div>
			<div class="page">
                    <webdiyer:AspNetPager ID="anpPage"  runat="server" AlwaysShow="true" FirstPageText="首页"
                        LastPageText="末页" PageSize="20" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0"
                        LayoutType="Table" NumericButtonCount="5" CustomInfoHTML="共 %PageCount% 页"
                        UrlPaging="false" onpagechanging="anpPage_PageChanging" ShowCustomInfoSection=Never>
                    </webdiyer:AspNetPager>
              </div>
			<div class="clear"></div>  
			<div class="newsBottom"><div class="clear"></div></div>
		<div class="clear"></div>
		</div>
		</form>
		<!--新闻公告结束-->
	<div class="clear"></div>
 	</div>
	<!--右边部分结束-->
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
