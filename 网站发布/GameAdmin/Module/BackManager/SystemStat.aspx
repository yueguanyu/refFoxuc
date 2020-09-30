<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="SystemStat.aspx.cs" Inherits="Game.Web.Module.BackManager.SystemStat" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <title></title>
    <style type="text/css">
        .gamelist
        {
            width: 650px;
        }
        .gamelist span
        {
            float: left;
            width: 200px;
            height: 23px;
            line-height: 19px;
            text-align: left;
            margin-top: 2px;
            margin-right: 0;
            margin-bottom: 2px;
            margin-left: 0;
        }
    </style>
</head>
<body>
    <form id="form1" runat="server">
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left">
                你当前位置：后台系统 - 系统统计
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" id="btnRefresh" class="btn wd1" value="刷新" onclick="javascript:location.href=location.href;" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    用户统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                在线人数：
            </td>
            <td>
                <asp:Literal ID="ltOnLineCount" runat="server"></asp:Literal> 个
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                停权用户：
            </td>
            <td>
                <asp:Literal ID="ltDisenableCount" runat="server"></asp:Literal> 个
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                注册总人数：
            </td>
            <td>
                <asp:Literal ID="ltAllCount" runat="server"></asp:Literal> 个
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    金币统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                金币总量：
            </td>
            <td>
                <asp:Literal ID="ltScore" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                银行总量：
            </td>
            <td>
                <asp:Literal ID="ltInsureScore" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                银行+金币总量：
            </td>
            <td>
                <asp:Literal ID="ltAllScore" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    赠送统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                注册赠送：
            </td>
            <td>
                <asp:Literal ID="ltRegGrantScore" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                泡分赠送：
            </td>
            <td>
                <asp:Literal ID="ltPresentScore" runat="server"></asp:Literal> 金币
            </td>
        </tr>
         <tr>
            <td class="listTdLeft">
                管理员后台手动赠送：
            </td>
            <td>
                <asp:Literal ID="ltManagerGrantScore" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    魅力统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                魅力总量：
            </td>
            <td>
                <asp:Literal ID="ltLoveLiness" runat="server"></asp:Literal>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                已兑换魅力总量：
            </td>
            <td>
                <asp:Literal ID="ltPresent" runat="server"></asp:Literal>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                未兑换魅力总量：
            </td>
            <td>
                <asp:Literal ID="ltRemainLove" runat="server"></asp:Literal>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                已兑换金币量：
            </td>
            <td>
                <asp:Literal ID="ltConvertPresent" runat="server"></asp:Literal>
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    税收统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                税收总量：
            </td>
            <td>
                <asp:Literal ID="ltRevenue" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                转账税收：
            </td>
            <td>
                <asp:Literal ID="ltTransferRevenue" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td valign="top" class="listTdLeft">
                游戏税收：<br />（金币）&nbsp;&nbsp;
            </td>
            <td>
                <div class="gamelist">
                    <asp:Repeater ID="rptGameTax" runat="server">
                        <ItemTemplate>
                            <span>
                                <%# GetGameKindName(int.Parse( Eval( "KindID" ).ToString( )))%>(<%# Eval( "Revenue" ).ToString( )%>) </span>
                        </ItemTemplate>
                    </asp:Repeater>
                </div>
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3 pd7">
                </div>
            </td>
        </tr>
        <tr>
            <td valign="top" class="listTdLeft">
                房间税收：<br />（金币）&nbsp;&nbsp;
            </td>
            <td>
                <div class="gamelist">
                    <asp:Repeater ID="rptRoomTax" runat="server">
                        <ItemTemplate>
                            <span>
                                <%#GetGameRoomName(int.Parse( Eval( "ServerID" ).ToString( )))%>(<%# Eval( "Revenue" ).ToString( )%>)</span>
                        </ItemTemplate>
                    </asp:Repeater>
                </div>
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    损耗统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                损耗总量：
            </td>
            <td>
                <asp:Literal ID="ltWaste" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td valign="top" class="listTdLeft">
                游戏损耗：<br />（金币）&nbsp;&nbsp;
            </td>
            <td>
                <div class="gamelist">
                    <asp:Repeater ID="rptGameWast" runat="server">
                        <ItemTemplate>
                            <span>
                                <%# GetGameKindName(int.Parse( Eval( "KindID" ).ToString( )))%>(<%# Eval( "Waste" ).ToString( )%>)</span>
                        </ItemTemplate>
                    </asp:Repeater>
                </div>
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3 pd7">
                </div>
            </td>
        </tr>
        <tr>
            <td valign="top" class="listTdLeft">
                房间损耗：<br />（金币）&nbsp;&nbsp;
            </td>
            <td>
                <div class="gamelist">
                    <asp:Repeater ID="rptRoomWast" runat="server">
                        <ItemTemplate>
                            <span>
                                <%#GetGameRoomName(int.Parse( Eval( "ServerID" ).ToString( )))%>(<%# Eval( "Waste" ).ToString( )%>)</span>
                        </ItemTemplate>
                    </asp:Repeater>
                </div>
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    点卡统计</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                生成张数：
            </td>
            <td>
                <asp:Literal ID="ltCardCount" runat="server"></asp:Literal> 张
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                金币总量：
            </td>
            <td>
                <asp:Literal ID="ltCardGold" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                面额总量：
            </td>
            <td>
                <asp:Literal ID="ltCardPrice" runat="server"></asp:Literal> 元
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                充值张数：
            </td>
            <td>
                <asp:Literal ID="ltCardPayCount" runat="server"></asp:Literal> 张
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                充值金币：
            </td>
            <td>
                <asp:Literal ID="ltCardPayGold" runat="server"></asp:Literal> 金币
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                充值人民币总数：
            </td>
            <td>
                <asp:Literal ID="ltCardPayPrice" runat="server"></asp:Literal> 元
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                会员卡张数：
            </td>
            <td>
                <asp:Literal ID="ltMemberCardCount" runat="server"></asp:Literal> 张
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" id="btnRefresh1" class="btn wd1" value="刷新" onclick="javascript:location.href=location.href;" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
