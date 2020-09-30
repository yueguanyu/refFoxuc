----------------------------------------------------------------------
-- 版权：2013
-- 时间：2013-02-20
-- 用途：玩家申请教程权限
----------------------------------------------------------------------
USE QPNativeWebDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_GetJiaoChengShengHe') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_GetJiaoChengShengHe					
GO

----------------------------------------------------------------------
CREATE PROC NET_PW_GetJiaoChengShengHe
	
WITH ENCRYPTION AS

BEGIN
	-- 属性设置
	SET NOCOUNT ON
	
	
SELECT  JiaoChengQuanXianID, news.NewsID, subject,JiaoChengQuanXian.UserID, GameID,Accounts,ShengQingDate, ShengQingIP, ShengHeDate, ShengHeiIP, ShengHeiRen, isShengHe
FROM QPNativeWebDB.dbo.JiaoChengQuanXian,QPAccountsDB.dbo.AccountsInfo,QPNativeWebDB.dbo.news
where  QPNativeWebDB.dbo.JiaoChengQuanXian.UserID=QPAccountsDB.dbo.AccountsInfo.UserID
and   QPNativeWebDB.dbo.JiaoChengQuanXian.NewsID=QPNativeWebDB.dbo.news.NewsID
ORDER BY  ShengQingDate DESC,isShengHe DESC 

END

