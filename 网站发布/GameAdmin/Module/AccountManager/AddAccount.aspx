<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="AddAccount.aspx.cs" Inherits="Game.Web.Module.AccountManager.AddAccount" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
    <link href="../../styles/layout.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript" src="../../scripts/common.js"></script>

    <script type="text/javascript" src="../../scripts/comm.js"></script>

    <script type="text/javascript" src="../../scripts/jquery.js"></script>

    <script type="text/javascript" src="../../scripts/jquery.validate.js"></script>

    <script type="text/javascript" src="../../scripts/messages_cn.js"></script>

    <script type="text/javascript" src="../../scripts/jquery.metadata.js"></script>

    <script type="text/javascript" src="../../scripts/My97DatePicker/WdatePicker.js"></script>

    <link type="text/css" href="../../scripts/lhgdialog/lhgdialog.css" rel="stylesheet" />

    <script src="../../scripts/lhgdialog/lhgcore.min.js" type="text/javascript"></script>

    <script src="../../scripts/lhgdialog/lhgdialog.js" type="text/javascript"></script>

    <script type="text/javascript">
        $(document).ready(function() {
            //弹出页面
            J('#btnSwitchFace').dialog({ id: 'winUserfaceList', title: '更换头像', width: 540, height: 385, page: '../../Tools/UserfacesList.aspx', rang: true, cover: true });
        })		        
    </script>

</head>
<body>
    <!-- 头部菜单 Start -->
    <table width="100%" border="0" cellpadding="0" cellspacing="0" class="title">
        <tr>
            <td width="19" height="25" valign="top" class="Lpd10">
                <div class="arr">
                </div>
            </td>
            <td width="1232" height="25" valign="top" align="left">
                目前操作功能：添加用户信息
            </td>
        </tr>
    </table>
    <!-- 头部菜单 End -->
    <form runat="server" id="form1" method="post">
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="关闭" class="btn wd1" onclick="window.close();" />
                <asp:Button ID="btnSave" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0" class="listBg2">
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    基本信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                用户名：
            </td>
            <td>
                <asp:TextBox ID="txtAccount" runat="server" CssClass="text wd4" MaxLength="20"></asp:TextBox><span class="hong">*</span>&nbsp;用户名字符长度不可超过31个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                用户昵称：
            </td>
            <td>
                <asp:TextBox ID="txtNickName" runat="server" CssClass="text wd4" MaxLength="20"></asp:TextBox>&nbsp;用户昵称字符长度不可超过31个字符，不填则与用户名相同
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                登录密码：
            </td>
            <td>
                <asp:TextBox ID="txtLogonPass" TextMode="Password" runat="server" CssClass="text wd4" MaxLength="20"></asp:TextBox><span
                    class="hong">*</span>&nbsp;登录密码字符长度不可超过32个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                确认登录密码：
            </td>
            <td>
                <asp:TextBox ID="txtLogonPass2" TextMode="Password" runat="server" CssClass="text wd4" MaxLength="20"></asp:TextBox><span
                    class="hong">*</span>&nbsp;请再次输入登录密码
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                银行密码：
            </td>
            <td>
                <asp:TextBox ID="txtInsurePass" TextMode="Password" runat="server" CssClass="text wd4" MaxLength="20"></asp:TextBox>&nbsp;银行密码字符长度不可超过32个字符，不填则与登录密码相同
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                确认银行密码：
            </td>
            <td>
                <asp:TextBox ID="txtInsurePass2" TextMode="Password" runat="server" CssClass="text wd4" MaxLength="20"></asp:TextBox>&nbsp;请再次输入银行密码
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                个性签名：
            </td>
            <td>
                <asp:TextBox ID="txtUnderWrite" runat="server" CssClass="text" Style="width: 500px" MaxLength="63"></asp:TextBox>&nbsp;个性签名字符长度不可超过63个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                性别：
            </td>
            <td>
                <asp:DropDownList ID="ddlGender" runat="server">
                    <asp:ListItem Value="1" Selected="True">男</asp:ListItem>
                    <asp:ListItem Value="2">女</asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                头像：
            </td>
            <td>
                <input id="inFaceID" name="inFaceID" type="hidden" value="" />
                <img id="picFace" alt="头像" title="头像" src="/gamepic/face1.gif" />&nbsp;&nbsp;
                <a href="javascript:void(0)" id="btnSwitchFace">查看更多头像</a>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                选项：
            </td>
            <td>
                <asp:CheckBox ID="ckbNullity" runat="server" Text="冻结帐号" />
                <asp:CheckBox ID="ckbStunDown" runat="server" Text="安全关闭" />
                <asp:CheckBox ID="chkIsAndroid" runat="server" Text="设为机器人" />
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                锁定机器：
            </td>
            <td>
                <asp:RadioButtonList ID="rdoMoorMachine" runat="server" RepeatDirection="Horizontal">
                    <asp:ListItem Value="0" Selected="True">未锁定</asp:ListItem>
                    <asp:ListItem Value="1">客户端锁定</asp:ListItem>
                    <asp:ListItem Value="2">网页锁定</asp:ListItem>
                </asp:RadioButtonList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                经验值：
            </td>
            <td>
                <asp:TextBox ID="txtExperience" runat="server" CssClass="text wd4" MaxLength="20" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>
            </td>
        </tr>
        <tr style="display:none;">
            <td class="listTdLeft">
                礼 物：
            </td>
            <td>
                <asp:TextBox ID="txtPresent" runat="server" CssClass="text wd4" MaxLength="20" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                魅力值：
            </td>
            <td>
                <asp:TextBox ID="txtLoveLiness" runat="server" CssClass="text wd4" MaxLength="20" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                会员级别：
            </td>
            <td>
                <asp:DropDownList ID="ddlMemberOrder" runat="server">
                   
                </asp:DropDownList>
                &nbsp;&nbsp;&nbsp;&nbsp;
                <span id="spanMember" style="display: none;">会员到期时间：
                    <asp:TextBox ID="txtMemberOverDate" runat="server" CssClass="text wd2"></asp:TextBox><img src="../../Images/btn_calendar.gif"
                        onclick="WdatePicker({el:'txtMemberOverDate',skin:'whyGreen',dateFmt:'yyyy-MM-dd'})" style="cursor: pointer; vertical-align: middle" />
                </span>
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    详细信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                真实姓名：
            </td>
            <td>
                <asp:TextBox ID="txtCompellation" runat="server" CssClass="text wd4" MaxLength="16"></asp:TextBox>&nbsp;真实姓名字符长度不可超过16个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                QQ号码：
            </td>
            <td>
                <asp:TextBox ID="txtQQ" runat="server" CssClass="text wd4" MaxLength="16" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>&nbsp;QQ字符长度不可超过16个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                电子邮箱：
            </td>
            <td>
                <asp:TextBox ID="txtEMail" runat="server" CssClass="text wd4" MaxLength="32" validate="email:true"></asp:TextBox>&nbsp;电子邮箱字符长度不可超过32个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                固定电话：
            </td>
            <td>
                <asp:TextBox ID="txtSeatPhone" runat="server" CssClass="text wd4" MaxLength="32"></asp:TextBox>&nbsp;固定电话字符长度不可超过32个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                手机号码：
            </td>
            <td>
                <asp:TextBox ID="txtMobilePhone" runat="server" CssClass="text wd4" MaxLength="16" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>&nbsp;手机号码字符长度不可超过16个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                邮政编码：
            </td>
            <td>
                <asp:TextBox ID="txtPostalCode" runat="server" CssClass="text wd4" MaxLength="8" onkeyup="if(isNaN(value))execCommand('undo');"></asp:TextBox>&nbsp;邮政编码字符长度不可超过8个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                居住地址：
            </td>
            <td>
                <asp:TextBox ID="txtDwellingPlace" runat="server" CssClass="text wd4" Style="width: 500px" MaxLength="128"></asp:TextBox>&nbsp;长度不可超过128个字符
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                用户备注：
            </td>
            <td>
                <asp:TextBox ID="txtUserNote" runat="server" CssClass="text wd4" Style="width: 500px" MaxLength="256"></asp:TextBox>&nbsp;长度不可超过256个字符
            </td>
        </tr>
        <tr>
            <td height="35" colspan="2" class="f14 bold Lpd10 Rpd10">
                <div class="hg3  pd7">
                    权限信息</div>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                <%--全选<input type="checkbox" onclick="CheckAll();" />--%>
                用户权限：
            </td>
            <td>
                <asp:CheckBoxList ID="ckbUserRight" runat="server" RepeatDirection="Horizontal" RepeatColumns="6">
                </asp:CheckBoxList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                用户身份：
            </td>
            <td>
                &nbsp;<asp:DropDownList ID="ddlMasterOrder" runat="server">
                    <asp:ListItem Value="0" Text="玩家"></asp:ListItem>
                    <asp:ListItem Value="1" Text="管理员"></asp:ListItem>
                </asp:DropDownList>
            </td>
        </tr>
        <tr>
            <td class="listTdLeft">
                <%-- 全选<input type="checkbox" onclick="SelectAllTable(this.checked,'masterOrder');" />--%>
                管理权限：
            </td>
            <td>
                <asp:CheckBoxList ID="ckbMasterRight" runat="server" RepeatDirection="Horizontal" RepeatColumns="6">
                </asp:CheckBoxList>
            </td>
        </tr>
    </table>
    <table width="99%" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
            <td class="titleOpBg Lpd10">
                <input type="button" value="关闭" class="btn wd1" onclick="window.close();" />
                <asp:Button ID="btnSave1" runat="server" Text="保存" CssClass="btn wd1" OnClick="btnSave_Click" />
            </td>
        </tr>
    </table>
    </form>
</body>
</html>

<script type="text/javascript">
    $(document).ready(function() {
        jQuery.metadata.setType("attr", "validate");
        jQuery("#<%=form1.ClientID %>").validate();



        $("#<%=ddlMemberOrder.ClientID %>").change(function() {
            if ($(this).val() == "0")
                $("#spanMember").css("display", "none");
            else
                $("#spanMember").css("display", "");
        });
    });
</script>

