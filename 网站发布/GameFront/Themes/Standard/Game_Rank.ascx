<%@ Control Language="C#" AutoEventWireup="true" CodeBehind="Game_Rank.ascx.cs" Inherits="Game.Web.Themes.Standard.Game_Rank" %>
<%@ Import Namespace="Game.Facade" %>

<!--游戏列表开始-->
        <asp:Repeater ID="rptGameTypes" runat="server" onitemdatabound="rptGameTypes_ItemDataBound">
            <ItemTemplate>
                <div class="gameListTitle1" id="S_<%# Container.ItemIndex+1 %>" onclick="JavaScript:showHide(document.getElementById('M_<%# Container.ItemIndex+1 %>'));"><label class="gameType"><%# Eval("TypeName") %></label></div>
                <div class="clear"></div>
                <ul id="M_<%# Container.ItemIndex+1 %>">
                    <asp:Repeater ID="rptGameList" runat="server">
                        <ItemTemplate>
                                <li><span><a href='/GameChart.aspx?KindID=<%# Eval("KindID") %>' class="lh"><%#  Game.Utils.TextUtility.CutStringProlongSymbol( Eval( "KindName" ).ToString( ) , 30 )%></a></span></li>
                        </ItemTemplate>
                    </asp:Repeater>
                </ul>
            </ItemTemplate>
        </asp:Repeater>	    		
	<!--游戏列表结束-->
	
	<input id="TCount" type="hidden" runat="server" value="3" />
	
	
	<script type="text/javascript">    
    function setCookie2(sName, sValue) {
        var expires = new Date();
        expires.setTime(expires.getTime() + 16 * 60 * 1000);
        document.cookie = sName + "=" + escape(sValue) + "; expires=" + expires.toGMTString() + "; path=/";
    }
    
    function getCookie (sName) {
	    var aCookie = document.cookie.split("; ");
	    for (var i=0; i < aCookie.length; i++) {
		    var aCrumb = aCookie[i].split("=");
		    if (sName == aCrumb[0])
			    return unescape(aCrumb[1]);
	    }
	    return null;
    }
    
    function showHide(obj) {
        var oStyle = obj.style;
        var imgId = obj.id.replace("M","S");
//        oStyle.display == "none" ? oStyle.display = "block" : oStyle.display = "none";
//        oStyle.display == "none" ? document.getElementById(imgId).className = "gameListTitle2" : document.getElementById(imgId).className = "gameListTitle1";
        
        if(oStyle.display == "none")
        {
            oStyle.display = "block";
            document.getElementById(imgId).className = "gameListTitle1";
            setCookie2("G"+obj.id,"on")
        }
        else
        {
            oStyle.display = "none";
            document.getElementById(imgId).className = "gameListTitle2";
            setCookie2("G"+obj.id,"off")
        }
    }    
    window.onload = function() {
        var count = document.getElementById("<%= TCount.ClientID %>").value;
        for(var i=1;i<=count;i++)
        {
            if (getCookie("GM_"+i) == null || getCookie("GM_"+i) == undefined || getCookie("GM_"+i) == "on") 
            {
                document.getElementById("M_"+i).style.display = "block";
                document.getElementById("S_"+i).className = "gameListTitle1";
            }
            else
            {
                document.getElementById("M_"+i).style.display = "none";
                document.getElementById("S_"+i).className = "gameListTitle2";
            }
        }
    }
</script>
