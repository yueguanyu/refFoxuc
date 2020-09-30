
----------------------------------------------------------------------------------------------------

USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchStart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchStart]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchOver]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchOver]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 比赛开始
CREATE PROC GSP_GR_MatchStart
	@wMatchID		INT,		-- 比赛标识
	@wMatchNo		INT,		-- 比赛场次
	@wMatchCount	INT,		-- 比赛总场
	@lInitScore		INT			-- 初始积分
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	update  GameScoreInfo set score=@lInitScore,wincount=0,lostcount=0,drawcount=0,fleecount=0
	delete GameHistoryScore WHERE MatchID=@wMatchID AND MatchNo=@wMatchNo
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- 比赛开始
CREATE PROC GSP_GR_MatchOver
	@wMatchID		INT,		-- 比赛标识
	@wMatchNo		INT,		-- 比赛场次
	@MatchCount	INT 		-- 比赛总场
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
		INSERT INTO GameHistoryScore
		(MatchID,MatchNo,UserID,HScore,HWinCount,HLostCount,HDrawCount,HFleeCount )  
	SELECT
		@wMatchID AS MatchID,@wMatchNo AS MatchNo,
		UserID,Score,WinCount,LostCount,DrawCount,FleeCount
	FROM GameScoreInfo(NOLOCK)
	
	SELECT TOP(3) A.NickName,B.HScore 
		FROM QPAccountsDB..AccountsInfo(NOLOCK) AS A,GameHistoryScore(NOLOCK) AS B
		WHERE A.UserID=B.UserID AND MatchID=@wMatchID AND MatchNo=@wMatchNo ORDER BY HScore desc,HWinCount 

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------