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

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_ShengQingJiaoChengQuanXianByID') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_ShengQingJiaoChengQuanXianByID					
GO

----------------------------------------------------------------------
CREATE PROC NET_PW_ShengQingJiaoChengQuanXianByID
	@dwNewsID		INT,				--	新闻标识
	@dwUserID		INT	,			--	用户标识	
	@ShengQingIP    varchar(20),--IP
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

BEGIN
	-- 属性设置
	SET NOCOUNT ON
	DECLARE @JiaoChengQuanXianID INT
	--DECLARE @JiaoChengQuanXianID1 INT
		
	IF  EXISTS ( select * FROM JiaoChengQuanXian where (NewsID=@dwNewsID AND UserID=@dwUserID ))	
	begin--存在已申请了
		--SELECT @JiaoChengQuanXianID1=JiaoChengQuanXianID FROM JiaoChengQuanXian where (NewsID=@dwNewsID AND UserID=@dwUserID and isShengHe=0)
		--IF (@JiaoChengQuanXianID1!=null)--申请了但没有审枋		
		SET @strErrorDescribe=N'已经申请，无需再申请'
		return 1	
	end
	else		
	begin 
		INSERT INTO [QPNativeWebDB].[dbo].[JiaoChengQuanXian]
               ([NewsID],[UserID],[ShengQingDate],[ShengQingIP])          
		VALUES (@dwNewsID,@dwUserID,getdate(),@ShengQingIP)
		SET @strErrorDescribe=N'申请成功'        
		return 0  
	end

END

