<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="IssueList.aspx.cs" Inherits="Game.Web.Service.IssueList" %>

<%@ Import Namespace="Game.Facade" %>
<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/customer_layout.css" />
</head>
<body>
    <qp:Header ID="sHeader" runat="server" />
    <!--页面主体开始-->
    <div class="main">
        <div class="customerBody">
            <form id="form1" runat="server">
            <div class="customerTitle">
            </div>
            <div class="customerBg">
                <div class="cLeft">
                    <div class="cLeftTop">
                    </div>
                    <div>
                        <a href="/Service/Index.aspx" class="xsbz" hidefocus="true"></a>
                    </div>
                    <div class="cjwt">
                    </div>
                    <div>
                        <a href="/Service/FeedbackList.aspx" class="wtfk" hidefocus="true"></a>
                    </div>
                    <div>
                        <a href="/Service/Customer.aspx" class="kfdh" hidefocus="true"></a>
                    </div>
                    <div class="cLeftBottom">
                    </div>
                    <div class="clear">
                    </div>
                </div>
                <div class="cRight">
                    <ul>
                        <asp:Repeater ID="rptIssueList" runat="server">
                            <ItemTemplate>
                                <li>
                                    <label>
                                    </label>
                                    <a href='/Service/IssueView.aspx?XID=<%# Eval("IssueID") %>' class="lh">
                                        <%# Eval("IssueTitle")%></a></li>
                            </ItemTemplate>
                        </asp:Repeater>
                    </ul>
                </div>
                <div class="mtop13" style="text-align: right; width: 70%;">
                    <webdiyer:AspNetPager ID="anpPage" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页" PageSize="20" NextPageText="下页"
                        PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table" NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%"
                        UrlPaging="false" OnPageChanging="anpPage_PageChanging">
                    </webdiyer:AspNetPager>
                </div>
                <div class="clear">
                </div>
            </div>
            </form>
            <div class="customerBottom">
            </div>
            <div class="clear">
            </div>
        </div>
    </div>
    <div class="clear">
    </div>
    <!--页面主体结束-->
    <qp:Footer ID="sFooter" runat="server" />
</body>
</html>
