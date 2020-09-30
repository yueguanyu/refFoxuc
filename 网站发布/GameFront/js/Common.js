/*
CreatDate: 2012-01-10
Discription: 常用JS库
*/

//计算字符串字节数，中文算两个字节
function GetByteLength(str) 
{
    var len = str.length;
    var byteLen = 0;
    for (var i = 0; i < len; i++) 
    {
        if (str.charCodeAt(i) < 27 || str.charCodeAt(i) > 126) 
        {
            byteLen += 2;
        } 
        else 
        {
            byteLen++;
        }
    }
    return byteLen;
}

//截取字符串，中文算两个字节
function SubString(str, len, hasDot) 
{
    var newLength = 0;
    var newStr = "";
    var chineseRegex = /[^\x00-\xff]/g;
    var singleChar = "";
    var strLength = str.replace(chineseRegex, "**").length;
    for (var i = 0; i < strLength; i++) 
    {
        singleChar = str.charAt(i).toString();
        if (singleChar.match(chineseRegex) != null) 
        {
            newLength += 2;
        }
        else 
        {
            newLength++;
        }
        if (newLength > len) 
        {
            break;
        }
        newStr += singleChar;
    }
    if (hasDot && strLength > len) 
    {
        newStr += "..";
    }
    return newStr;
}