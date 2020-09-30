<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GamePropertyInfo.aspx.cs" Inherits="Game.Web.Module.AppManager.GamePropertyInfo" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>

    <script type="text/javascript" src="../../scripts/jquery.js"></script>

    <script type="text/javascript" src="../../scripts/jquery.validate.js"></script>

    <script type="text/javascript" src="../../scripts/messages_cn.js"></script>

    <script type="text/javascript" src="../../scripts/jquery.metadata.js"></script>

    <script type="text/javascript">
        jQuery(document).ready(function() {
            jQuery.metadata.setType("attr", "validate");
            jQuery.validator.addMethod("decmal1", function(value, element) {
                var decmal = /^\d+(\.\d+)?$/;
                return decmal.test(value) || this.optional(element);
            });
            jQuery("#<%=form1.ClientID %>").validate();
        });
    </script>

    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left">
                你当前位置：系统维护 - 道具管理
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('GamePropertyManager.aspx');" />
                <asp:Button ID="btnSave" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    道具信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                道具名称：
            </td>
            <td>
                <asp:TextBox ID="txtName" runat="server" CssClass="text" validate="{required:true}"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                现金价格：
            </td>
            <td>
                <asp:TextBox ID="txtCash" runat="server" CssClass="text" validate="{decmal1:true}"></asp:TextBox>（网站使用）
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                金币价格：
            </td>
            <td>
                <asp:TextBox ID="txtGold" runat="server" CssClass="text" validate="{digits:true}" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                折扣：
            </td>
            <td>
                <asp:TextBox ID="txtDiscount" runat="server" CssClass="text" MaxLength="3" validate="{digits:true,range:[0,100]}" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>%
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                赠送魅力值：
            </td>
            <td>
                <asp:TextBox ID="txtSendLoveLiness" runat="server" CssClass="text" validate="{digits:true}" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                接收魅力值：
            </td>
            <td>
                <asp:TextBox ID="txtRecvLoveLiness" runat="server" CssClass="text" validate="{digits:true}" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                发行范围：
            </td>
            <td>
                <asp:CheckBoxList ID="ckbIssueArea" runat="server" RepeatDirection="Horizontal">
                </asp:CheckBoxList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                使用范围：
            </td>
            <td>
                <asp:CheckBoxList ID="ckbServiceArea" runat="server" RepeatDirection="Horizontal">
                </asp:CheckBoxList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                使用说明：
            </td>
            <td>
                <asp:TextBox ID="txtRegulationsInfo" runat="server" CssClass="text" Width="300" MaxLength="256"></asp:TextBox>&nbsp;使用说明字符长度不可超过256个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                是否启用：
            </td>
            <td>
                <asp:CheckBox ID="ckbNullity" runat="server" Text="启用" />
            </td>
        </tr>
    </table>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="返回" class="btn wd1" onclick="Redirect('GamePropertyManager.aspx');" />
                <asp:Button ID="btnSave1" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>
