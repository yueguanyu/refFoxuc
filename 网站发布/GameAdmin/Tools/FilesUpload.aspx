<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="FilesUpload.aspx.cs" Inherits="Game.Web.Tools.FilesUpload" %>
<%@ Import Namespace="Game.Utils" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <link href="../styles/layout.css" rel="stylesheet" type="text/css" />
    <script type="text/javascript" src="../scripts/common.js"></script>
    <script type="text/javascript" src="../scripts/JQuery.js"></script>
    <style type="text/css">
        html, body, form
        {
            margin: 0px;
            padding: 0px;
            font-family:宋体;
            font-size: 12px;
            background: #caebfc;
        }
        #fileUpload 
        {
        	width:400px;        	
        }
    </style>
    
    <script type="text/javascript">
        function uploadMsg() {
            showInfo("正在上传文件中...");
        }

        function checkedUpload() {
            var sFile = $("#fileUpload").val();

            if (sFile.length == 0) {
                showError("请选择需要上传的文件");
                return false;
            }

            return true;
        }

        /*
        * 设置文件上传信息
        */
        function setUploadFilePath(inImgInstalledUrl, uploadPath) {
            parent.document.getElementById(inImgInstalledUrl).value = uploadPath;
            setUploadFileView(uploadPath);
        }

        /*
        * 设置文件浏览
        */
        function setUploadFileView(uploadPath) {
            var uploadArray = uploadPath.split('/');
            $("#pnFileName").text("文件名：" + uploadArray[uploadArray.length - 1]);            
        }
    </script>    
    <title>文件上传</title>
</head>
<body>
    <form id="fmUpload" method="post" action="?act=upload&amp;path=<%=Utility.UrlEncode(clientPath) %>&amp;inImgUrl=<%=inImgUrl %>" enctype="multipart/form-data" onsubmit="return checkedUpload();">        
    <input type="file" name="fileUpload" id="fileUpload" size="23" class="text" style=" height:22px;" />
    <input id="btnUpload" type="submit" name="btnUpload" value="上传" class="btn" style="width:60px;" onclick="uploadMsg();" />
    
    <span id="pnFileName"></span>    
    </form>    
    
    <form id="fmUploadSecond" runat="server"></form>   
</body>
</html>

