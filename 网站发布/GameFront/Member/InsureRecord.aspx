<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="InsureRecord.aspx.cs" Inherits="Game.Web.Member.InsureRecord" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Info" Src="~/Themes/Standard/User_Info.ascx" %>
<%@ Register TagPrefix="qp" TagName="Logon" Src="~/Themes/Standard/User_Logon.ascx" %>
<%@ Register TagPrefix="qp" TagName="UserSidebar" Src="~/Themes/Standard/User_Sidebar.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/user_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/game_layout.css" />   

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

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="userBody" style="margin-top:8px;">
	<div class="userTop"></div>
	<div class="userBg">
	    <!--左边页面开始-->
	    <div class="LeftSide">
			<% if (Fetch.GetUserCookie() == null)
           { %>
        <qp:Logon ID="sLogon" runat="server" />
        <%}
           else
           { %>
        <qp:Info ID="sInfo" runat="server" />
        <% } %>
			
<div class="LeftSide mtop10">

<qp:UserSidebar ID="sUserSidebar" runat="server" />

</div>		
<div class="clear"></div>					
		</div>
		<!--左边页面结束-->
		
		<!--右边页面开始-->		
		<div class="mainRight2">
			<div class="userRigthBody">
				<div class="userRigthTitle">会员中心 - 保险柜服务 - 交易明细</div>
				<div class="userRigthBg">
				<!--存款开始-->
	            <div class="title f14 bold lan">交易明细</div>
				  	<br />
				  	<form name="form1" action="" runat="server">
				  	<table width="100%" border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td align="right" style="width:60px;">日期查询：</td>
                        <td align="left">
                            
                            <asp:TextBox ID="txtStartDate" runat="server" CssClass="text" Width="70" MaxLength="10" onfocus="WdatePicker({skin:'whyGreen',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox>
                            <img src="/Images/btn_calendar.gif" onclick="WdatePicker({el:'txtStartDate',skin:'whyGreen',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})" style="cursor:pointer;vertical-align:text-bottom"/>  
                            
                            至
                            
                            <asp:TextBox ID="txtEndDate" runat="server" CssClass="text" Width="70" MaxLength="10" onfocus="WdatePicker({skin:'whyGreen',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox>
                            <img src="/Images/btn_calendar.gif" onclick="WdatePicker({el:'txtEndDate',skin:'whyGreen',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})" style="cursor:pointer;vertical-align:text-bottom"/>                    
                            
                            <asp:DropDownList ID="ddlType" runat="server" Width="80">
                                <asp:ListItem Value="0" Selected="True" Text="全部"></asp:ListItem>
                                <asp:ListItem Value="1" Text="存款"></asp:ListItem>
                                <asp:ListItem Value="2" Text="取款"></asp:ListItem>
                                <asp:ListItem Value="3" Text="转账（出）"></asp:ListItem>
                                <asp:ListItem Value="4" Text="转账（入）"></asp:ListItem>
                            </asp:DropDownList>
                           <asp:Button ID="btnSelect" Text="查 询" runat="server" CssClass="bnt" 
                                onclick="btnSelect_Click" />
                        </td>
                      </tr>
                    </table>
                    
			  	    <table width="100%" border="0" cellpadding="0" cellspacing="1" bgcolor="#BAD7E5" class="leftTop">
                      <tr align="center" class="bold">
                        <td class="leftTop trBg">交易日期</td>
                        <td class="leftTop trBg">汇款人</td>
                        <td class="leftTop trBg">收款人</td>
                        <td class="leftTop trBg">交易类别</td>
                        <td class="leftTop trBg">交易金额</td>
                        <td class="leftTop trBg">服务费</td>  
                        <td class="leftTop trBg">交易场所</td>       
                        <td class="leftTop trBg">房间</td>                        
                      </tr>   
                          <asp:Repeater ID="rptInsureList" runat="server">
                            <ItemTemplate>
                                <tr align="center" class="trBg1">
                                    <td><%# Eval("CollectDate")%></td>  
                                    <td><%# GetAccountsByUserID(Convert.ToInt32(Eval("SourceUserID")))%></td>  
                                    <td><%# GetAccountsByUserID(Convert.ToInt32(Eval("TargetUserID")))%></td>  
                                    <td><%# (Convert.ToInt32(Eval("TradeType")) == 1 ? "存款" : Convert.ToInt32(Eval("TradeType")) == 2 ? "取款" : Convert.ToInt32(Eval("TradeType")) == 3 && int.Parse(Eval("TargetUserID").ToString()) != userTicket.UserID ? "转出" : "转入")%></td>
                                    <td><%# (int.Parse(Eval("TradeType").ToString()) == 3 && int.Parse(Eval("TargetUserID").ToString()) != userTicket.UserID) ? (0 - double.Parse(Eval("SwapScore").ToString())) : double.Parse(Eval("SwapScore").ToString()) %></td>
                                    <td><%# Eval("Revenue")%></td>
                                    <td><%# (Convert.ToInt32(Eval("IsGamePlaza"))==0?"大厅":"网页") %></td> 
                                    <td><%# (Convert.ToInt32(Eval("IsGamePlaza"))==0?Eval("ServerID"):"") %></td>
                                </tr>
                            </ItemTemplate>
                          </asp:Repeater>
                          <tr id="trNoData" runat="server"><td bgcolor='#FFFFFF' colspan='100' align='center'><br>没有任何信息!<br><br></td></tr>
                    </table>
                    <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                        <tr>
                            <td class="page2" align="right">
                                <webdiyer:AspNetPager ID="anpPage"  runat="server" AlwaysShow="true" FirstPageText="首页"
                                    LastPageText="末页" PageSize="20" NextPageText="下页" PrevPageText="上页" ShowBoxThreshold="0" ShowCustomInfoSection="Left"
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
				<div class="userRightBottom"><div class="clear"></div></div>
				<div class="clear"></div>
			</div>
			<div class="clear"></div>
		</div>
		<!--右边页面结束-->
		<div class="clear"></div>
	</div>
	<div class="userBottom"><div class="clear"></div></div>
<div class="clear"></div>
</div>
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
