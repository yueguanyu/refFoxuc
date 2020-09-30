
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载配置
CREATE PROC GSP_GR_LoadParameter
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT							-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 奖牌汇率
	DECLARE @MedalRate AS INT
	SELECT @MedalRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalRate'

	-- 银行税率
	DECLARE @RevenueRate AS INT
	SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRate'

	-- 参数调整
	IF @MedalRate IS NULL SET @MedalRate=1
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- 程序版本
	DECLARE @ClientVersion AS INT
	DECLARE @ServerVersion AS INT
	SELECT @ClientVersion=TableGame.ClientVersion, @ServerVersion=TableGame.ServerVersion
	FROM QPPlatformDBLink.QPPlatformDB.dbo.GameGameItem TableGame,QPPlatformDBLink.QPPlatformDB.dbo.GameKindItem TableKind
	WHERE TableGame.GameID=TableKind.GameID	AND TableKind.KindID=@wKindID

	-- 输出结果
	SELECT @MedalRate AS MedalRate, @RevenueRate AS RevenueRate, @ClientVersion AS ClientVersion, @ServerVersion AS ServerVersion

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------