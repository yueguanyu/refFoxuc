USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- I D 登录
CREATE PROC GSP_GR_MatchReward
	@dwUserID INT,								-- 用户 I D
	@dwMatchID INT,								-- 比赛 I D 
	@dwMatchNO	INT,							-- 比赛场次
	@wRank INT,									-- 比赛名次
	@lMatchScore INT,							-- 比赛得分
	@dwExperience INT,							-- 奖励经验
	@dwGold INT,								-- 奖励金币
	@dwMedal INT,								-- 奖励金牌
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15)					-- 连接地址
WITH ENCRYPTION AS

-- 执行逻辑
BEGIN
	
	UPDATE QPAccountsDB..AccountsInfo SET UserMedal=UserMedal+@dwMedal,Experience=Experience+@dwExperience WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
		RETURN 1
	
	UPDATE QPTreasureDB..GameScoreInfo SET Score=Score+@dwGold WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		INSERT QPTreasureDB..GameScoreInfo (UserID,Score,LastLogonIP)
								VALUES(@dwUserID,@dwGold,@strClientIP)
	END
	
	INSERT INTO StreamMatchHistory (UserID,MatchID,MatchNO,Rank,MatchScore,Experience,Gold,Medal,KindID,ServerID,ClientIP)
					VALUES(@dwUserID,@dwMatchID,@dwMatchNO,@wRank,@lMatchScore,@dwExperience,@dwGold,@dwMedal,@wKindID,@wServerID,@strClientIP)

END

RETURN 0
GO