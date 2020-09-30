<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Customer.aspx.cs" Inherits="Game.Web.Service.Customer" %>
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
    <link rel="stylesheet" type="text/css" href="/style/customer_layout.css" />
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main">
	<div class="customerBody">
		<div class="customerTitle"></div>
		<div class="customerBg">
			<div class="cLeft">
				<div class="cLeftTop"></div>
				<div><a href="/Service/Index.aspx" class="xsbz" hidefocus="true"></a></div>
				<div><a href="/Service/IssueList.aspx" class="cjwt" hidefocus="true"></a></div>
				<div><a href="/Service/FeedbackList.aspx" class="wtfk" hidefocus="true"></a></div>
				<div class="kfdh"><a href="/Service/Customer.aspx" hidefocus="true"></a></div>
				<div class="cLeftBottom"></div>
				<div class="clear"></div>
			</div>			
			<div class="cRight" style="font-family:'宋体'"> <br />
			  <div align="center" class=" f14 bold">联系方式</div>
			  <table width="92%" border="0" align="center" cellpadding="0" cellspacing="0">
                <tr>
                  <td>&nbsp;</td>
                  <td>&nbsp;</td>
                </tr>
                <tr>
                  <td width="30%" align="right"><strong>联系地址</strong>：</td>
                  <td width="70%">深圳市福田区北环大道 7043 号青海大厦 712 室</td>
                </tr>
                <tr>
                  <td align="right"><strong>邮政编码</strong>：</td>
                  <td>518034 </td>
                </tr>
                <tr style="border-bottom:1px dashed #ccc;">
                  <td align="right" style="border-bottom:1px dashed #ccc;">&nbsp;</td>
                  <td style="border-bottom:1px dashed #ccc;">&nbsp;</td>
                </tr>
                <tr>
                  <td align="right">&nbsp;</td>
                  <td>&nbsp;</td>
                </tr>
                <tr>
                  <td align="right"><strong>联系人</strong>：</td>
                  <td>易小姐 </td>
                </tr>
                <tr>
                  <td align="right"><strong>联系电话</strong>： </td>
                  <td>0755-83547940 转 603</td>
                </tr>
                <tr>
                  <td align="right" style="height: 18px"><strong>手机</strong>：</td>
                  <td style="height: 18px">13689535625</td>
                </tr>
                <tr>
                  <td align="right"><strong>QQ</strong>：</td>
                  <td>516022666</td>
                </tr>
                <tr>
                  <td align="right"><strong>MSN</strong>：</td>
                  <td>yizhaoping@foxuc.cn</span></td>
                </tr>
                <tr>
                  <td align="right"><strong>E-mail</strong>：</td>
                  <td>694645386@qq.com</td>
                </tr>
                <tr>
                  <td align="right" style="border-bottom:1px dashed #ccc;">&nbsp;</td>
                  <td style="border-bottom:1px dashed #ccc;">&nbsp;</td>
                </tr>
                <tr>
                  <td align="right">&nbsp;</td>
                  <td>&nbsp;</td>
                </tr>
                <tr>
                  <td align="right"><strong>联系人</strong>：</td>
                  <td>熊先生</td>
                </tr>
                <tr>
                  <td align="right"><strong>联系电话</strong>：</td>
                  <td>0755-83547940 转 603 </td>
                </tr>
                <tr>
                  <td align="right"><strong>手机</strong>：</td>
                  <td>15019203285</td>
                </tr>
                <tr>
                  <td align="right"><strong>QQ</strong>：</td>
                  <td>371037888</td>
                </tr>
                <tr>
                  <td align="right"><strong>E-mail</strong>：</td>
                  <td>ario@foxuc.cn </td>
                </tr>
              </table>
			  <br />
              <br />
              <div class="clear"></div>
	  	  </div>
		</div>
		<div class="customerBottom"></div>
		<div class="clear"></div>
	</div>
	<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
