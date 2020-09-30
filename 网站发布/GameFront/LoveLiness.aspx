<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="LoveLiness.aspx.cs" Inherits="Game.Web.LoveLiness" %>

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id=Head1 runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
      
    <link rel="stylesheet" type="text/css" href="/style/client.css" /> 
    
    <script src="../js/jquery-1.5.2.min.js" type="text/javascript"></script>
    <script src="/JS/formValidator.js" type="text/javascript"></script>
    <script src="/JS/formValidatorRegex.js" type="text/javascript"></script>
    
    <script type="text/javascript">
        /*$(document).ready(function(){
            //页面验证
           $.formValidator.initConfig({formid:"form1",alertmessage:true,onerror:function(msg){alert(msg)}});
            $("#txtPresent").formValidator()
                .regexValidator({regexp:"intege1",datatype:"enum",onerror:"兑换的魅力点必须为正整数！"});            
        })*/
    </script>
</head>
<body>
<div class="userRigthBg loveMain">
    <!--兑换开始-->
    <div class="title">魅力兑换</div>
    <br />
    <form id="form1" name="form1" runat="server">
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5">
        <tr>
        <td height="139" background="/images/contestBg1.jpg">
            <table width="96%" border="0" align="center" cellpadding="0" cellspacing="0" >
              <tr style="display:none;">
                <td width="25%" align="right">ID：</td>
                <td width="27%"><asp:Label ID="lblGameID" runat="server"></asp:Label></td>
                <td width="17%" align="right" class="lineBottom">帐　号：</td>
                <td width="31%"><asp:Label ID="lblAccounts" runat="server"></asp:Label></td>
              </tr>
              <tr>                            
                <td align="right" width="25%">总魅力值：</td>
                <td width="27%"><asp:Label ID="lblTotalLoves" runat="server"></asp:Label></td>
                <td align="right" width="17%">已兑换魅力值：</td>
                <td width="31%"><asp:Label ID="lblExchangeLoves" runat="server"></asp:Label></td>
              </tr>
              <tr>
                <td align="right">剩余魅力值：</td>
                <td><asp:Label ID="lblUnExchangeLoves" runat="server" Text="0"></asp:Label></td>
                <td align="right">保险柜金币值：</td>
                <td><asp:Label ID="lblInsureScore" runat="server" Text="0"></asp:Label></td>
              </tr>
            </table>
        </td>
        </tr>
    </table>
    <br />
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
      <tr>
        <td>
        <table width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr class="bold">
            <td width="28%" align="right">将</td>
            <td width="26%" align="center"><asp:TextBox ID="txtPresent" runat="server" CssClass="textTwo"></asp:TextBox></td>
            <td width="46%" align="left">魅力点兑换成游戏币</td>
          </tr>
        </table>
        </td>
      </tr>
      <tr>
        <td align="center"><asp:Button ID="btnUpdate" Text="兑 换" runat="server" CssClass="bnt lan bold" onclick="btnUpdate_Click" />
        </td>
      </tr>
    </table>
    </form>
    <br />
    <div class="title">温馨提示</div>
    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
       <tr>
         <td width="10%" align="right"></td>
         <td width="90%" class="hui6">
            1、魅力点直接兑换到保险柜金币。<br>
            2、有任何问题，请您联系客服中心。<br>
         </td>
       </tr>
    </table>     
    <!--兑换结束-->
    <div class="clear"></div>
    </div>
</body>
</html>
