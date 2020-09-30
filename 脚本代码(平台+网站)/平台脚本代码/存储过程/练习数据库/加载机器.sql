
----------------------------------------------------------------------------------------------------

USE QPEducateDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadAndroidUser
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载机器
CREATE PROC GSP_GR_LoadAndroidUser
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT							-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询机器
	SELECT UserID, MinPlayDraw, MaxPlayDraw, MinTakeScore, MaxTakeScore, MinReposeTime, MaxReposeTime,
		ServiceTime, ServiceGender FROM AndroidManager(NOLOCK)
	WHERE (ServerID=@wServerID OR ServerID=0) AND Nullity=0 ORDER BY NEWID()

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------