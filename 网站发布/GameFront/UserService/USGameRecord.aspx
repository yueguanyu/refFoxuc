<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="USGameRecord.aspx.cs" Inherits="Game.Web.UserService.USGameRecord" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id=Head1 runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    <link rel="stylesheet" type="text/css" href="/style/client.css" />
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
 <div class="userRigthBg2">
<!--存款开始-->
	  	<br />
	  	<form id=Form1 name="form1" action="" runat="server">
	  	<div>
	  	<table width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td align="right" style="width:60px;">日期：</td>
            <td align="left">
               <asp:TextBox ID="txtStartDate" runat="server" CssClass="text " Width="70" MaxLength="10" onfocus="WdatePicker({skin:'client',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})"></asp:TextBox>
                <img src="/Images/btn_calendar.gif" onclick="WdatePicker({el:'txtStartDate',skin:'client',dateFmt:'yyyy-MM-dd',maxDate:'#F{$dp.$D(\'txtEndDate\')}'})" style="cursor:pointer;vertical-align:text-bottom"/>  
                
                至
                
                <asp:TextBox ID="txtEndDate" runat="server" CssClass="text" Width="70" MaxLength="10" onfocus="WdatePicker({skin:'client',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})"></asp:TextBox>
                <img src="/Images/btn_calendar.gif" onclick="WdatePicker({el:'txtEndDate',skin:'client',dateFmt:'yyyy-MM-dd',minDate:'#F{$dp.$D(\'txtStartDate\')}'})" style="cursor:pointer;vertical-align:text-bottom"/>
                                        
                <asp:Button ID="btnSelect" Text="查 询" runat="server" CssClass="bnt" 
                    onclick="btnSelect_Click" /> 
            </td>
          </tr>
        </table>
        </div>
  	    <table width="100%" border="0" cellpadding="0" cellspacing="1" class="leftTop" bgcolor="#BAD7E5">
          <tr align="center" class="bold">
            <td class="leftTop trBg">开始时间</td>
            <td class="leftTop trBg">结束时间</td>
            <td class="leftTop trBg">游戏</td>
            <!--<td class="leftTop trBg">房间</td>-->
            <%--<td class="leftTop trBg">损耗</td>--%>
            <td class="leftTop trBg">税收</td>
            <!--<td class="leftTop trBg">赠送</td> -->
            <!--<td class="leftTop trBg">用户数</td>-->
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
                        <!--<td style="text-align:center;"><%# GetServerNameByServerId( Convert.ToInt32( Eval( "ServerID" ) ) )%></td>-->
                        <%--<td style="text-align:center;"><%# Eval("Waste")%></td>--%>
                       <td style="text-align:center;"><%# Eval("Revenue")%></td>
                       <!--<td style="text-align:center;"><%# Eval("UserMedal") %></td>-->
                        <!--<td style="text-align:center;"><%# Eval("UserCount")%></td>-->
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
