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

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_GetJiaoChengStates') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_GetJiaoChengStates					
GO

----------------------------------------------------------------------
CREATE PROC NET_PW_GetJiaoChengStates
	@dwNewsID		INT,				--	新闻标识
	@dwUserID		INT	,			--	用户标识		
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

BEGIN
	-- 属性设置
	SET NOCOUNT ON
	DECLARE @DanJian INT
	--DECLARE @JiaoChengQuanXianID1 INT
	select @DanJian=jiaoChengRMB from news where newsID=@dwNewsID
	if @DanJian=0
	begin --单价为0的为免费学习的
		SET @strErrorDescribe=N'2'
		return 2
	END
		
	IF  EXISTS ( select * FROM JiaoChengQuanXian where (NewsID=@dwNewsID AND UserID=@dwUserID ))	
	begin--存在已申请了
		IF  EXISTS ( select * FROM JiaoChengQuanXian where (NewsID=@dwNewsID AND UserID=@dwUserID and isShengHe=1))
		BEGIN --已经审核了可以学习
			SET @strErrorDescribe=N'2'
			return 2	
		end
		else
		begin--申请了但没审核		
			SET @strErrorDescribe=N'1'
			return 1	
		end
	end
	else--没申请		
	begin 
		SET @strErrorDescribe=N'0'      
		return 0  
	end

END

