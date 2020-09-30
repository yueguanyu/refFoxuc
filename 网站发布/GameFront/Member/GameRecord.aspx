<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GameRecord.aspx.cs" Inherits="Game.Web.Member.GameRecord" %>
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

    <script src="/js/jquery-1.5.2.min.js" type="text/javascript"></script>
    
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
        
        function showInfo(obj)
        {
            var ID = "DrawID"+obj.id;
            if(document.getElementById(ID).style.display == "none")
            {
                document.getElementById(ID).style.display = "";
                obj.src = "/Images/up1.gif"
            }  
            else
            {
                document.getElementById(ID).style.display="none";
                obj.src = "/Images/down1.gif"
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
				<div class="userRigthTitle">会员中心 - 会员服务 - 游戏记录</div>
				<div class="userRigthBg">
				<!--存款开始-->
	            <div class="title f14 bold lan">游戏记录</div>
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
                                                    
                            <asp:Button ID="btnSelect" Text="查 询" runat="server" CssClass="bnt" 
                                onclick="btnSelect_Click" />
                        </td>
                      </tr>
                    </table>

			  	    <table width="100%" border="0" cellpadding="0" cellspacing="1" class="leftTop" bgcolor="#BAD7E5">
                      <tr align="center" class="bold">
                        <td class="leftTop trBg">开始时间</td>
                        <td class="leftTop trBg">结束时间</td>
                        <td class="leftTop trBg">游戏</td>
                        <td class="leftTop trBg">房间</td>
                        <td class="leftTop trBg">税收</td>
                        <td class="leftTop trBg">奖牌</td>                               
                      </tr>   
                          <asp:Repeater ID="rptDrawInfoList" runat="server" 
                                onitemdatabound="rptDrawInfoList_ItemDataBound">
                            <ItemTemplate>
                                <tr  class="trBg1">
                                    <td style="text-align:center;"><%# Eval("StartTime")%>
                                        &nbsp;<img id="<%# Eval( "DrawID" ).ToString( )%>" src="/Images/down1.gif" style="cursor: pointer" title="查看同桌玩家"
                                        onclick="showInfo(this)" />
                                    </td>
                                    <td style="text-align:center;"><%# Eval("ConcludeTime")%></td>
                                    <td style="text-align:center;"><%# GetGameNameByKindId(Convert.ToInt32(Eval( "KindID" )))%></td>
                                    <td style="text-align:center;"><%# GetServerNameByServerId( Convert.ToInt32( Eval( "ServerID" ) ) )%></td>
                                    <td style="text-align:center;"><%# Eval("Revenue")%></td>
                                    <td style="text-align:center;"><%# Eval("UserMedal") %></td>
                                  </tr>
                                  <tr style="display: none" id="DrawID<%# Eval( "DrawID" ).ToString( )%>">
                                    <td colspan="8">
                                        <table width="100%" border="0" align="center" style="background-color:White;" cellpadding="0" cellspacing="0">
                                            <tr align="center"class="bold">
                                                <td class="trBg">
                                                    用户名
                                                </td>
                                                <td class="leftTop trBg">
                                                    输赢金额
                                                </td>
                                                <td class="leftTop trBg">
                                                    税收
                                                </td>
                                                <td class="leftTop trBg">
                                                    奖牌
                                                </td>
                                                <td class="leftTop trBg">
                                                    椅子编号
                                                </td>
                                            </tr>
                                            <asp:Repeater ID="rptDrawScoreList" runat="server">
                                                <ItemTemplate>
                                                    <tr class="list" align="center">
                                                        <td>
                                                            <%# GetNickNameByUserID( int.Parse( Eval( "UserID" ).ToString( ) ) )%>
                                                        </td>
                                                        <td>
                                                            <%# Eval( "Score" ).ToString( )%>
                                                        </td>
                                                        <td>
                                                            <%# Eval( "Revenue" ).ToString( )%>
                                                        </td>
                                                         <td>
                                                            <%# Eval( "UserMedal" ).ToString( )%>
                                                        </td>
                                                        <td>
                                                            <%# Eval( "ChairID" ).ToString( )%>
                                                        </td>
                                                    </tr>
                                                </ItemTemplate>
                                            </asp:Repeater>
                                        </table>
                                    </td>
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
