<%@ Control Language="C#" AutoEventWireup="true" %>

<div class="clear"></div>
<div class="serve mtop10">
  <div class="serve1 bold hui3 ">玩家排行榜</div>
	<div class="tab">
		<div class="tab1" id="divScore">财富排行</div>
		<div class="tab2" id="divLoves">魅力排行</div>
	</div>
	<div class="topBg">
		<ul id="ulOrderBy">
            
		</ul>					
	<div class="clear"></div>
	</div>
	<div class="clear"></div>
</div>
<script type="text/javascript">
    $(document).ready(function() {
        $.ajaxSetup({ global: false });
        GetScoreInfo();
        $("#divScore").click(function() {
            $(this).removeClass("tab2");
            $(this).addClass("tab1");

            $("#divLoves").removeClass("tab1");
            $("#divLoves").addClass("tab2");

            GetScoreInfo();
        });

        $("#divLoves").click(function() {
            $(this).removeClass("tab2");
            $(this).addClass("tab1");

            $("#divScore").removeClass("tab1");
            $("#divScore").addClass("tab2");
            GetUserLoves();
        });
    });

    function GetScoreInfo() {
        $.ajax({
            contentType: "application/json",
            url: "WS/WSTreasure.asmx/GetUserScoroInfo",
            data: "{}",
            type: "POST",
            dataType: "json",
            success: function(json) {
                json = eval("(" + json.d + ")");
                $("#ulOrderBy li").remove();
                if (json.length == 0) return;
                for (var i = 0; i < json.length; i++) {
                    $("#ulOrderBy").append("<li title=\"" + json[i].userName + "\"><span class=\"hui6\">" + json[i].s + "</span>" + (json[i].userName.length > 7 ? (json[i].userName.substring(0, 7) + "..") : json[i].userName) + "</li>");
                }
            },
            error: function(err, ex) {
                //alert(err.responseText);
            }
        });
    }

    function GetUserLoves() {
        $.ajax({
            contentType: "application/json",
            url: "WS/WSAccount.asmx/GetUserLoves",
            data: "{}",
            type: "POST",
            dataType: "json",
            success: function(json) {
                json = eval("(" + json.d + ")");
                $("#ulOrderBy li").remove();
                if (json.length == 0) return;
                for (var i = 0; i < json.length; i++) {
                    $("#ulOrderBy").append("<li title=\"" + json[i].userName + "\"><span class=\"hui6\">" + json[i].loves + "</span>" + (json[i].userName.length > 7 ? (json[i].userName.substring(0, 7) + "..") : json[i].userName) + "</li>");
                }
            },
            error: function(err, ex) {
                //alert(err.responseText);
            }
        });
    }
</script>