<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="PayYB.aspx.cs" Inherits="Game.Web.Pay.PayYB" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>
<%@ Register TagPrefix="qp" TagName="Btn" Src="~/Themes/Standard/Common_Btn.ascx" %>
<%@ Register TagPrefix="qp" TagName="Question" Src="~/Themes/Standard/Common_Question.ascx" %>
<%@ Register TagPrefix="qp" TagName="Service" Src="~/Themes/Standard/Common_Service.ascx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
    
    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/pay_layout.css"  />
    
    <script src="/js/jquery-1.5.2.min.js" type="text/javascript"></script>
    <script src="/js/utils.js" type="text/javascript"></script>
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--页面主体开始-->
<div class="main" style="margin-top:8px;">
<!--左边部分开始-->
<!--左边部分开始-->
<div class="mainLeft1">
	<div class="LeftSide">		
		<qp:Btn ID="sBtn" runat="server" />
		
	    <qp:Question ID="sQuestion" runat="server" />
		
		<qp:Service ID="sService" runat="server" />
		
	<div class="clear"></div>
	</div>
	<div class="clear"></div>
</div>
<!--左边部分结束-->
<!--左边部分结束-->

<!--右边开始-->
<div id="content">
    <!--右边内容头部-->
    <div class="top">您的位置：首页&nbsp;<span class="arrow">»</span>&nbsp;充值中心&nbsp;<span class="arrow">»</span>&nbsp;帐号充值</div>
    <!--右边内容中部-->
    <div class="center">
    <div class="step2"></div>
	<div class="payRechargeTitle" style="text-align:left; padding-top:5px;">您选择了<span class="hong">“网银”</span>充值方式</div>
	
	
	<form name="fmStep1" runat="server" id="fmStep1">

    <script type="text/javascript">
    $(document).ready(function() {
        $("#rbTimeTypeM").attr("checked", "checked");
        $("#rbTimeM1").attr("checked", "checked");
        replaceClassTime("rbTimeM1");
        $("#hdfSalePrice").val("10");
        $("#txtTimeM0").val("");

        $("#txtPayAccounts").blur(function() { checkAccounts(); });
        $("#txtPayReAccounts").blur(function() { checkReAccounts(); });

        $("#btnPay").click(function() {
            return checkInput();
        });

        $("#rbTimeTypeM").click(function() {
            $("#rbTimeTypeM").parent().addClass("hand");
            $("#rbTimeTypeM").parent().removeClass("gray");

            $("#rbTimeTypeY").parent().addClass("gray");
            $("#rbTimeTypeY").parent().removeClass("hand");

            if ($("#lblTimeM").is(":visible") == false) {
                $("#rbTimeM1").attr("checked", "checked");
                replaceClassTime("rbTimeM1");
                $("#hdfSalePrice").val("10");
                $("#txtTimeM0").val("");
                $("#ePayInfo").hide("");
                $("#lblPayInfo").html("付款金额：10 元，赠送金币 100,000 金币");
            }

            $("#lblTimeM").show();
            $("#lblTimeY").hide();
        });
        $("#rbTimeTypeY").click(function() {
            $("#rbTimeTypeM").parent().addClass("gray");
            $("#rbTimeTypeM").parent().removeClass("hand");

            $("#rbTimeTypeY").parent().addClass("hand");
            $("#rbTimeTypeY").parent().removeClass("gray");

            if ($("#lblTimeY").is(":visible") == false) {
                $("#rbTimeY1").attr("checked", "checked");
                replaceClassYear("rbTimeY1");
                $("#lblPayInfo").html("付款金额：120 元，赠送金币 1,200,000 金币");
                $("#hdfSalePrice").val("120");
            }
            $("#ePayInfo").hide();
            $("#lblTimeY").show();
            $("#lblTimeM").hide();
        });

        $("#rbTimeM1").click(function() {
            replaceClassTime("rbTimeM1");
            $("#lblPayInfo").html("付款金额：10 元，赠送金币 100,000 金币");
            $("#txtTimeM0").val("");
            $("#ePayInfo").hide("");
            $("#hdfSalePrice").val("10");
        });
        $("#rbTimeM3").click(function() {
            replaceClassTime("rbTimeM3");
            $("#lblPayInfo").html("付款金额：30 元，赠送金币 300,000 金币");
            $("#txtTimeM0").val("");
            $("#ePayInfo").hide("");
            $("#hdfSalePrice").val("30");
        });
        $("#rbTimeM6").click(function() {
            replaceClassTime("rbTimeM6");
            $("#lblPayInfo").html("付款金额：60 元，赠送金币 600,000 金币");
            $("#txtTimeM0").val("");
            $("#ePayInfo").hide("");
            $("#hdfSalePrice").val("60");
        });
        $("#rbTimeM0").click(function() {
            replaceClassTime("rbTimeM0");
            $("#lblPayInfo").html("付款金额：0 元，赠送 0 金币");
            $("#txtTimeM0").focus();
            $("#hdfSalePrice").val("0");
            $("#ePayInfo").show("");
            $("#ePayInfo").html("&nbsp;&nbsp;请输入1~24间的数字");
        });
        $("#txtTimeM0").focus(function() {
            $("#rbTimeM0").attr("checked", "checked");
            replaceClassTime("rbTimeM0");
            if ($(this).val() == "") {
                $("#lblPayInfo").html("付款金额：0 元，赠送 0 金币");
                $("#hdfSalePrice").val("0");
                $("#ePayInfo").show("");
                $("#ePayInfo").html("&nbsp;&nbsp;请输入1~24间的数字");
            }
        }).keyup(function() {
            $("#ePayInfo").show("");
            var re = /\d/;
            if (!re.test($(this).val())) {
                $("#ePayInfo").html("&nbsp;&nbsp;开通时长必须为数字");
                $(this).focus();
                $("#hdfSalePrice").val("0");
                return;
            }
            if (parseInt($(this).val()) < 1 || parseInt($(this).val()) > 24) {
                $("#ePayInfo").html("&nbsp;&nbsp;请输入1~24间的数字");
                $(this).focus();
                $("#hdfSalePrice").val("0");
                return;
            }
            $("#ePayInfo").hide("");
            var txt = parseInt($(this).val());
            if (txt < 10) {
                $("#lblPayInfo").html("付款金额：" + txt * 10 + " 元，赠送金币 " + txt + "00,000 金币");
            } else if (txt >= 10 && txt <= 24) {
                $("#lblPayInfo").html("付款金额：" + txt * 10 + " 元，赠送金币 " + txt.toString().substring(0, 1) + "," + txt.toString().substring(1, 2) + "00,000 金币");
            }
            $("#hdfSalePrice").val(parseInt($(this).val()) * 10);
        });

        //年
        $("#rbTimeY1").click(function() {
            replaceClassYear("rbTimeY1");
            $("#lblPayInfo").html("付款金额：120 元，赠送金币 1,200,000 金币");
            $("#hdfSalePrice").val("120");
        });
        $("#rbTimeY2").click(function() {
            replaceClassYear("rbTimeY2");
            $("#lblPayInfo").html("付款金额：240 元，赠送金币 2,400,000 金币");
            $("#hdfSalePrice").val("240");
        });
    });

    function replaceClassTime(obj) {
        $("#rbTimeM1").parent().removeClass("hand");
        $("#rbTimeM1").parent().addClass("gray");
        $("#rbTimeM3").parent().removeClass("hand");
        $("#rbTimeM3").parent().addClass("gray");
        $("#rbTimeM6").parent().removeClass("hand");
        $("#rbTimeM6").parent().addClass("gray");
        $("#rbTimeM0").parent().removeClass("hand");
        $("#rbTimeM0").parent().addClass("gray");

        if ("rbTimeM1" == obj) {
            $("#rbTimeM1").parent().removeClass("gray");
            $("#rbTimeM1").parent().addClass("hand");
        }
        if ("rbTimeM3" == obj) {
            $("#rbTimeM3").parent().removeClass("gray");
            $("#rbTimeM3").parent().addClass("hand");
        }
        if ("rbTimeM6" == obj) {
            $("#rbTimeM6").parent().removeClass("gray");
            $("#rbTimeM6").parent().addClass("hand");
        }
        if ("rbTimeM0" == obj) {
            $("#rbTimeM0").parent().removeClass("gray");
            $("#rbTimeM0").parent().addClass("hand");
        }
    }

    function replaceClassYear(obj) {
        if (obj == "rbTimeY1") {
            $("#rbTimeY1").parent().removeClass("gray");
            $("#rbTimeY1").parent().addClass("hand");

            $("#rbTimeY2").parent().removeClass("hand");
            $("#rbTimeY2").parent().addClass("gray");
        }
        if (obj == "rbTimeY2") {
            $("#rbTimeY1").parent().removeClass("hand");
            $("#rbTimeY1").parent().addClass("gray");

            $("#rbTimeY2").parent().removeClass("gray");
            $("#rbTimeY2").parent().addClass("hand");
        }
    }

    function hintMessage(hintObj, error, message) {
        //删除样式
        if (error == "error") {
            $("#" + hintObj + "").removeClass("onCorrect");
            $("#" + hintObj + "").removeClass("onFocus");
            $("#" + hintObj + "").addClass("onError");
        } else if (error == "right") {
            $("#" + hintObj + "").removeClass("onError");
            $("#" + hintObj + "").removeClass("onFocus");
            $("#" + hintObj + "").addClass("onCorrect");
        } else if (error == "info") {
            $("#" + hintObj + "").removeClass("onError");
            $("#" + hintObj + "").addClass("onFocus");
            $("#" + hintObj + "").removeClass("onCorrect");
        }

        $("#" + hintObj + "").html(message);
    }

    function checkAccounts() {
        if ($.trim($("#txtPayAccounts").val()) == "") {
            $("#txtPayAccountsTips").html("请输入您的游戏账号");
            return false;
        }
        $("#txtPayAccountsTips").html("");
        return true;
    }

    function checkReAccounts() {
        if ($.trim($("#txtPayReAccounts").val()) == "") {
            $("#txtPayReAccountsTips").html("请再次输入游戏账号");
            return false;
        }
        if ($("#txtPayReAccounts").val() != $("#txtPayAccounts").val()) {
            $("#txtPayReAccountsTips").html("两次输入的账号不一致");
            return false;
        }
        $("#txtPayReAccountsTips").html("");
        return true;
    }

    function checkInput() {
        if (!checkAccounts()) { $("#txtPayAccounts").focus(); return false; }
        if (!checkReAccounts()) { $("#txtPayReAccounts").focus(); return false; }
        if (parseInt($("#hdfSalePrice").val()) <= 0) {
            $("#ePayInfo").show("");
            $("#ePayInfo").html("&nbsp;&nbsp;请输入1~24间的数字");
            $("#txtTimeM0").focus();
            return false;
        }
    }
    </script>


	<div class="content">
	    <div class="height10"></div>
        <p style="padding-bottom:8px;">
            <span style="padding-top:5px;">充值用户名：</span>
            <label><asp:TextBox ID="txtPayAccounts" runat="server" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
            <label id="txtPayAccountsTips" style="padding-top:5px; color:Red"></label>
        </p>
        <p style="padding-bottom:8px;">
            <span style="padding-top:5px;">确认用户名：</span>
            <label><asp:TextBox ID="txtPayReAccounts" runat="server" CssClass="text width206" Height="15"></asp:TextBox>&nbsp;&nbsp;</label>
            <label id="txtPayReAccountsTips" style="color:Red; padding-top:5px;"></label>
        </p>				
		<p>
		    <span style="padding-top:5px;">付费模式：</span>
		    <label class="type">                
		        <label class="hand"><input type="radio" id="rbTimeTypeM" name="rbTimeType" value="1" checked="checked" />按月付费</label>
		        <label class="gray"><input type="radio" id="rbTimeTypeY" name="rbTimeType" value="2" />按年付费<b style=" font-weight:normal; color:#FF3300;">(推荐)</b></label>
		    </label>
	    </p>				
		<p>
		    <span style="padding-top:5px;">开通时长：</span>
			<label class="type" id="lblTimeM">
			    <label class="hand"><input type="radio" id="rbTimeM1" name="rbTimeM" value="1" checked="checked"/>1个月</label>		
			    <label class="gray"><input type="radio" id="rbTimeM3" name="rbTimeM" value="3" />3个月</label>			  
			    <label class="gray"><input type="radio" id="rbTimeM6" name="rbTimeM" value="6" />6个月</label>			      	    
			    <label class="gray"><input type="radio" id="rbTimeM0" name="rbTimeM" value="0"/>其他 <input type="text" id="txtTimeM0" class="text" style="height:15px;" size="4" maxlength="2" /> 个月</label>
			</label>
			<label id="lblTimeY" style="display:none; padding-bottom:13px;">
			    <label class="hand"><input type="radio" id="rbTimeY1" name="rbTimeY" value="1" checked="checked"/>1年</label>				
			    <label class="gray"><input type="radio" id="rbTimeY2" name="rbTimeY" value="3" />2年</label>					  
			</label>
	    </p>				
	    <p style="padding-bottom:8px;">
	  		<span></span>
		    <label id="lblPayInfo">付款金额：10 元，赠送 100,000 金币</label>
		    <label id="ePayInfo" style="color:Red;"></label>
            <input type="hidden" name="hdfSalePrice" id="hdfSalePrice" runat="server" value="10" />
		</p>	
		<p class="height" style="display:none;">
	  		<span class="mtop10">支付方式：</span>
			<label class="kuang">
				<Label><input name="" type="radio" value="" checked="checked" class="mtop12"/></Label><b><img src="/images/99bill.gif"/></b>
			</label>
	  	</p>			
    </div>
	<div class="clear"></div>
	<div class="content_btn">
        <asp:Button ID="btnPay" runat="server" CssClass="btnL" Text="确 定" onclick="btnPay_Click" />
    </div>  
    <div class="pay_tips_t" style="text-align:left;">
		<span class="bold" style="padding-top:5px;">温馨提示：</span>
		<p style="padding-bottom:8px;">1、请确保您填写的用户名正确无误，因玩家输入错误而导致的任何损失由用户自行承担。</p>
		<p style="padding-bottom:8px;">2、充值过程中，浏览器会跳转至银行页面，支付成功后，会自动返回家园网站，如果没有跳转或是弹出充值成功的页面，请您不要关闭充值窗口。</p>
		<p>3、遇到任何充值问题，请您联系客服中心。</p>
	<div class="clear"></div>
	</div> 

	</form>
	
	<form id="fmStep2" runat="server">
     <div class="pay_tips">
        <p>
            <asp:Label ID="lblAlertIcon" runat="server"></asp:Label>
            <asp:Label ID="lblAlertInfo" runat="server" Text="操作提示"></asp:Label>
            <asp:Literal ID="lit99Bill" runat="server"></asp:Literal>
        </p>
        <p id="pnlContinue" runat="server">
            <input id="btnReset1" type="button" value="继续充值" onclick="goURL('/Pay/PayYB.aspx');" class="btn_home_4blue" />
        </p>
    </div>        
    </form>
    
	<div class="height10"></div>           
    <div class="clear"></div>
    </div>  	    
<div class="bottom"></div> 
</div>

</div>
   
<!--页面主体结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
