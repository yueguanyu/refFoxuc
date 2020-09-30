<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Index.aspx.cs" Inherits="Game.Web.Match.Index" %>

<%@ Import Namespace="Game.Facade" %>
<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
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
    <div class="main" style="margin-top: 8px;">
        <!--左边部分开始-->
        <!--左边部分开始-->
        <div class="mainLeft1">
            <div class="LeftSide">
                <qp:Btn ID="sBtn" runat="server" />
                <qp:Question ID="sQuestion" runat="server" />
                <qp:Service ID="sService" runat="server" />
                <div class="clear">
                </div>
            </div>
            <div class="clear">
            </div>
        </div>
        <!--左边部分结束-->
        <!--左边部分结束-->
        <!--右边部分开始-->
        <div class="mainRight1">
            <!--赛事列表开始-->
            <div class="newsBody">
                <div class="left">
                    <img src="/images/match.gif" /></div>
                <div class="tgBg">
                    <asp:Repeater ID="rptMatchList" runat="server">
                        <ItemTemplate>
                            <div class="tgTitle1" style="overflow:hidden;text-overflow:ellipsis;white-space:nowrap;">
                                <a href='/Match/MatchView.aspx?XID=<%# Eval("MatchID") %>' class="lh lan f14 bold" title=" <%# Eval("MatchName")%>">
                                    <%# Eval("MatchName")%></a></div>
                            <div class="content hui6 mbottom10" style="word-break: break-all; width: 600px;">
                                <%# Eval("MatchSummary")%></div>
                        </ItemTemplate>
                    </asp:Repeater>
                </div>
                <div class="clear">
                </div>
                <div class="newsBottom">
                    <div class="clear">
                    </div>
                </div>
                <div class="clear">
                </div>
            </div>
            <!--赛事列表结束-->
            <div class="clear">
            </div>
        </div>
        <!--右边部分结束-->
        <div class="clear">
        </div>
    </div>
    <!--页面主体结束-->
    <qp:Footer ID="sFooter" runat="server" />
</body>
</html>
