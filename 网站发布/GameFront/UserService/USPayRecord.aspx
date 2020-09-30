<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="USPayRecord.aspx.cs" Inherits="Game.Web.UserService.USPayRecord" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id=Head1 runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/client.css" />

    <script src="../js/jquery-1.5.2.min.js" type="text/javascript"></script>
    <script src="/JS/formValidatorRegex.js" type="text/javascript"></script>   
    <script src="/js/My97DatePicker/WdatePicker.js" type="text/javascript"></script>
    
    <script type="text/javascript">
        function QueryInfo()
        {            
            var startDate = document.form1.StartDate.value;
            var endDate = document.form1.EndDate.value;
            if(!isDate(startDate)||!isDate(endDate))
            {
                alert("日期格式不正确！");
                return false;
            }
        }
    </script>
</head>
<body>
<div class="userRigthBg2">
			<!--存款开始-->
			  	<br />
			  	<form id=Form1 name="form1" action="" runat="server">
			  	<table width="100%" border="0" cellspacing="0" cellpadding="0">
                  <tr>
                    <td align="right" style="width:60px;">日期：</td>
                    <td align="left">
                        
                        <asp:TextBox ID="txtStartDate" runat="server" CssClass="text" Width="70" MaxLength="10" onfocus="WdatePicker({skin:'client',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox>
                        <img src="/Images/btn_calendar.gif" onclick="WdatePicker({el:'txtStartDate',skin:'client',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})" style="cursor:pointer;vertical-align:text-bottom"/>  
                        至
                        <asp:TextBox ID="txtEndDate" runat="server" CssClass="text" Width="70" MaxLength="10" onfocus="WdatePicker({skin:'client',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox>
                        <img src="/Images/btn_calendar.gif" onclick="WdatePicker({el:'txtEndDate',skin:'client',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})" style="cursor:pointer;vertical-align:text-bottom"/>
                        <!--
                        <asp:DropDownList ID="ddlShareType" Width="80" runat="server">
                            <asp:ListItem Selected="True" Text="全部服务" Value="0"></asp:ListItem>
                            <asp:ListItem Text="实卡充值" Value="1"></asp:ListItem>
                            <asp:ListItem Text="快钱充值" Value="2"></asp:ListItem>
                            <asp:ListItem Text="易宝充值" Value="3"></asp:ListItem>
                        </asp:DropDownList>
                        -->
                        
                        <asp:Button ID="btnSelect" Text="查 询" runat="server" CssClass="bnt" 
                            onclick="btnSelect_Click" />
                    </td>
                  </tr>
                </table>
                
		  	    <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class="leftTop">
                  <tr align="center" class="bold">
                    <td class="leftTop trBg">充值日期</td>
                    <!--<td class="leftTop trBg">充值类型</td>-->
                    <td class="leftTop trBg">充值卡号</td>
                    <td class="leftTop trBg">订单号</td>
                    <!--<td class="leftTop trBg">卡类型</td>-->
                    <!--<td class="leftTop trBg">卡价格</td>-->  
                    <td class="leftTop trBg">赠送金币</td>       
                    <!--<td class="leftTop trBg">订单金额</td>-->                        
                    <td class="leftTop trBg">支付金额</td>                        
                  </tr>   
                      <asp:Repeater ID="rptPayList" runat="server">
                        <ItemTemplate>
                            <tr align="center" class="trBg1">
                                <td><%# Eval("ApplyDate","{0:yyyy-MM-dd}")%></td>
                                <!--<td><%# (Convert.ToInt32(Eval("ShareID")) == 1 ? "实卡充值" : Convert.ToInt32(Eval("ShareID"))==2?"快钱充值":"易宝充值")%></td>-->
                                <td><%# Eval("SerialID")%></td>
                                <td><%# Eval( "OrderID" )%></td>
                                <!--<td><%# Convert.ToInt32(Eval("CardTypeID")) == 1 ? "水晶月卡" : Convert.ToInt32(Eval("CardTypeID")) == 2 ? "黄金半年卡" : Convert.ToInt32(Eval("CardTypeID")) == 3 ? "白金年卡" : "超级会员"%></td>-->
                                <!--<td><%# Convert.ToInt32(Eval("CardPrice"))%></td>-->
                                <td><%# Eval("CardGold")%></td>
                                <!--<td><%# Eval("OrderAmount")%></td>-->
                                <td><%# Convert.ToInt32(Eval("PayAmount"))%></td>
                              </tr>
                        </ItemTemplate>
                      </asp:Repeater>
                  <tr id="trNoData" runat="server"><td bgcolor='#FFFFFF' colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>    
                </table>
                <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                    <tr>
                        <td class="page2" align="center">
                            <webdiyer:AspNetPager ID="anpPage"  runat="server" AlwaysShow="true" FirstPageText="首页"
                                LastPageText="末页" PageSize="8" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Never"
                                LayoutType="Table" NumericButtonCount="5" CustomInfoHTML="总记录：%RecordCount%　页码：%CurrentPageIndex%/%PageCount%　每页：%PageSize%"
                                UrlPaging="false" onpagechanging="anpPage_PageChanging">
                            </webdiyer:AspNetPager>
                        </td>
                    </tr>
                </table>  
                </form>
                
            <!--存款结束-->
            <div class="clear"></div>
	 	    </div>		
</body>
</html>
