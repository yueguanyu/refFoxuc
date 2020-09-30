<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="FeedbackList.aspx.cs" Inherits="Game.Web.Service.FeedbackList" %>

<%@ Import Namespace="Game.Facade" %>
<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

    <script src="/js/jquery-1.5.2.min.js" type="text/javascript"></script>

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
                    <div>
                        <a href="/Service/IssueList.aspx" class="cjwt" hidefocus="true"></a>
                    </div>
                    <div class="wtfk">
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
                    <br />
                    <table width="100%" border="0" cellpadding="0" cellspacing="0">
                        <tr align="center">
                            <td class="line">
                                提交者
                            </td>
                            <td class="line">
                                留言主题
                            </td>
                            <td class="line">
                                提交时间
                            </td>
                            <td class="line">
                                回复
                            </td>
                            <td class="line">
                                人气
                            </td>
                        </tr>
                        <asp:Repeater ID="rptFeedBackList" runat="server">
                            <ItemTemplate>
                                <tr align="center">
                                    <td>
                                        <%# (Eval("Accounts").ToString() == "" ? "匿名用户" : Eval("Accounts"))%>
                                    </td>
                                    <td>
                                        <a href="javascript:;" onclick="GetMessage('<%# Eval("FeedbackID") %>')" class="lh" title='<%# Eval("FeedbackTitle") %>'>
                                            <%# GetStringByObj(Eval("FeedbackTitle"),30) %></a>
                                    </td>
                                    <td>
                                        <%# Eval("FeedbackDate","{0:yyyy-MM-dd}")%>
                                    </td>
                                    <td>
                                        <%# Eval("RevertContent").ToString()==""?"未解决":"<font color='red'>已解决</font>" %>
                                    </td>
                                    <td id='id<%# Eval("FeedbackID") %>'>
                                        <%# Eval("ViewCount")%>
                                    </td>
                                </tr>
                            </ItemTemplate>
                        </asp:Repeater>
                    </table>
                </div>
                <div class="mtop13" style="text-align: right; width: 70%;float:right; padding-right:40px;">
                    <webdiyer:AspNetPager ID="anpPage" runat="server" AlwaysShow="true" FirstPageText="首页" LastPageText="末页" PageSize="10" NextPageText="下页"
                        PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left" LayoutType="Table" NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%"
                        UrlPaging="false" OnPageChanging="anpPage_PageChanging">
                    </webdiyer:AspNetPager>
                </div>
                <div class="cRight">
                    <br />
                    <div style="display: none;" id="tblFeed">
                        <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#C4CFD5" class="padding15">
                            <tr>
                                <td width="27%" align="right" bgcolor="#FFFFFF">
                                    留言者：
                                </td>
                                <td width="73%" bgcolor="#FFFFFF" id="lblUser">
                                </td>
                            </tr>
                            <tr>
                                <td align="right" bgcolor="#FFFFFF">
                                    留言内容：
                                </td>
                                <td bgcolor="#FFFFFF" id="lblContent" style="word-wrap: break-word; width: 500px;">
                                </td>
                            </tr>
                            <tr id="trMessage">
                                <td align="right" bgcolor="#F7F9FD" class="">
                                    管理员回复：
                                </td>
                                <td bgcolor="#F7F9FD" id="lblMessage" style="word-wrap: break-word; width: 500px;">
                                </td>
                            </tr>
                        </table>
                    </div>
                    <table width="100%" border="0" cellpadding="0" cellspacing="0" bgcolor="#eef2f9" class="mtop13">
                        <tr class="height">
                            <td width="23%" height="65" align="right">
                                用户名：
                            </td>
                            <td width="77%">
                                <asp:TextBox ID="txtAccounts" runat="server" CssClass="width206"></asp:TextBox>&nbsp; <font class="hui6">用户名为空则为匿名用户</font>
                            </td>
                        </tr>
                        <tr style="padding-bottom: 15px;">
                            <td align="right">
                                主题：
                            </td>
                            <td>
                                <asp:TextBox ID="txtTitle" runat="server" Width="400"></asp:TextBox>
                            </td>
                        </tr>
                        <tr>
                            <td align="right">
                                内容：
                            </td>
                            <td>
                                <asp:TextBox ID="txtContent" runat="server" Width="400" TextMode="MultiLine" Height="150"></asp:TextBox>
                            </td>
                        </tr>
                        <tr>
                            <td class="height">
                                &nbsp;
                            </td>
                            <td>
                                <asp:Button ID="btnPublish" runat="server" Text="发表留言" OnClientClick="return checkInput()" CssClass="btn1" OnClick="btnPublish_Click" />
                            </td>
                        </tr>
                    </table>
                    <br />
                    <div class="clear">
                    </div>
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

<script type="text/javascript">
    function checkInput() {
        if ($.trim($("#txtTitle").val()) == "") {
            alert("请输入主题！");
            $("#txtTitle").focus();
            return false;
        }
        if ($("#txtTitle").val().length > 512) {
            alert("主题长度不能超过512个字符！");
            return false;
        }

        if ($.trim($("#txtContent").val()) == "") {
            alert("请输入内容！");
            $("#txtContent").focus();
            return false;
        }
        if ($("#txtContent").val().length > 4000) {
            alert("内容长度不能超过4000个字符！");
            return false;
        }
    }

    var isExecute = true;
    var vFeedID = "";

    function GetMessage(obj) {
        if (!isExecute) return;

        if (vFeedID == obj) return;
        vFeedID = obj;

        isExecute = false;

        $.ajax({
            contentType: "application/json",
            url: "/WS/WSNativeWeb.asmx/GetFeedBack",
            data: "{feedID:'" + obj + "'}",
            type: "POST",
            dataType: "json",
            success: function(json) {
                json = eval("(" + json.d + ")");

                if (json.success == "error") {
                    $("#tblFeed").slideUp(200);
                    return;
                } else if (json.success == "success") {
                    $("#tblFeed").slideUp(100);
                    $("#tblFeed").slideDown(400);

                    $("#lblUser").html(json.userName);
                    $("#lblContent").html(json.fcon);
                    if (json.rcon == "")
                        $("#trMessage").hide();
                    else {
                        $("#trMessage").show();
                        $("#lblMessage").html(json.rcon);
                    }

                    $("#id" + obj).text(json.count);
                    window.setTimeout(function() { isExecute = true; }, 500);
                }
            },
            error: function(err, ex) {
                alert(err.responseText);
            }
        });
    }

    //更新验证码
    function UpdateImage() {
        document.getElementById("picVerifyCode").src = "../../ValidateImage.aspx?r=" + Math.random();
    }
</script>

