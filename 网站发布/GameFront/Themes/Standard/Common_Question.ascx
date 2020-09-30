<%@ Control Language="C#" AutoEventWireup="true" CodeBehind="Common_Question.ascx.cs" Inherits="Game.Web.Themes.Standard.Common_Question" %>
<%@ Import Namespace="Game.Facade" %>

<!--常见问题开始-->
<div class="serve mtop10">
  <div class="serve1 bold"><div class="bold hui3 left">常见问题</div><span><a href="/Service/IssueList.aspx" class="lh6">更多</a></span></div>
	<div class="faqBg">
		<ul>
            <asp:Repeater ID="rptIssueList" runat="server">
                <ItemTemplate>
                    <li><a href="/Service/IssueView.aspx?XID=<%# Eval("IssueID") %>" target="_blank" class="lh" ><%# Eval("IssueTitle")%></a></li>
                </ItemTemplate>
            </asp:Repeater>		
		</ul>				
	</div>
</div>
<!--常见问题结束-->
