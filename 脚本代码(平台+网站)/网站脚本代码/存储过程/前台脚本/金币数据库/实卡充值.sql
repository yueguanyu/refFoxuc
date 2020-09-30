----------------------------------------------------------------------
-- 版权：2011
-- 时间：2011-09-1
-- 用途：实卡充值
----------------------------------------------------------------------

USE [QPTreasureDB]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_FilledLivcard') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_FilledLivcard
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------
-- 实卡充值
CREATE PROC NET_PW_FilledLivcard
	@dwOperUserID		INT,						--	操作用户

	@strSerialID		NVARCHAR(32),				--	会员卡号
	@strPassword		NCHAR(32),					--	会员卡密	
	@strAccounts		NVARCHAR(31),				--	充值玩家帐号

	@strClientIP		NVARCHAR(15),				--	充值地址
	@strErrorDescribe	NVARCHAR(127) OUTPUT		--	输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 实卡信息
DECLARE @CardID INT
DECLARE @SerialID NVARCHAR(15)
DECLARE @Password NCHAR(32)
DECLARE @CardTypeID INT
DECLARE @CardPrice DECIMAL(18,2)
DECLARE @CardGold INT
DECLARE @MemberOrder TINYINT
DECLARE @MemberDays INT
DECLARE @UserRight INT
DECLARE @ValidDate DATETIME
DECLARE @ApplyDate DATETIME
DECLARE @UseRange INT

-- 帐号资料
DECLARE @Accounts NVARCHAR(31)
DECLARE @GameID INT
DECLARE @UserID INT
DECLARE @SpreaderID INT
DECLARE @Nullity TINYINT
DECLARE @StunDown TINYINT
DECLARE @WebLogonTimes INT
DECLARE @GameLogonTimes INT

-- 会员资料
DECLARE @MaxMemberOrder INT
DECLARE @MaxUserRight INT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 执行逻辑
BEGIN
	DECLARE @ShareID INT
	SET @ShareID=1		-- 1 实卡
	
	-- 卡号查询
	SELECT	@CardID=CardID,@SerialID=SerialID,@Password=[Password],@CardTypeID=CardTypeID,
			@CardPrice=CardPrice,@CardGold=CardGold,@MemberOrder=MemberOrder,@MemberDays=MemberDays,
			@UserRight=UserRight,@ValidDate=ValidDate,@ApplyDate=ApplyDate,@UseRange=UseRange,@Nullity=Nullity
	FROM LivcardAssociator WHERE SerialID = @strSerialID

	-- 验证卡信息
	IF @CardID IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉！您要充值的卡号不存在。如有疑问请联系客服中心。'
		RETURN 101
	END	

	IF @strPassword=N'' OR @strPassword IS NULL OR @Password<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'抱歉！充值失败，请检查卡号或密码是否填写正确。如有疑问请联系客服中心。'
		RETURN 102
	END

	IF @ApplyDate IS NOT NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉！该充值卡已被使用，请换一张再试。如有疑问请联系客服中心。'
		RETURN 103
	END

	IF @Nullity=1
	BEGIN
		SET @strErrorDescribe=N'抱歉！该会员卡已被禁用。'
		RETURN 104
	END

	IF @ValidDate < GETDATE()
	BEGIN
		SET @strErrorDescribe=N'抱歉！该会员卡已经过期。'
		RETURN 105
	END
	
	-- 验证用户
	SELECT @UserID=UserID,@GameID=GameID,@Accounts=Accounts,@Nullity=Nullity,@StunDown=StunDown,@SpreaderID=SpreaderID,
		   @WebLogonTimes=WebLogonTimes,@GameLogonTimes=GameLogonTimes
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo
	WHERE Accounts=@strAccounts

	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉！您要充值的用户账号不存在。'
		RETURN 201
	END

	IF @Nullity=1
	BEGIN
		SET @strErrorDescribe=N'抱歉！您要充值的用户账号暂时处于冻结状态，请联系客户服务中心了解详细情况。'
		RETURN 202
	END

	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉！您要充值的用户账号使用了安全关闭功能，必须重新开通后才能继续使用。'
		RETURN 203
	END

	-- 实卡使用范围
	-- 新注册用户
	IF @UseRange = 1
	BEGIN
		IF @WebLogonTimes+@GameLogonTimes>1
		BEGIN
			SET @strErrorDescribe=N'抱歉！该会员卡只适合新注册的用户使用。'
			RETURN 301
		END 
	END
	-- 第一次充值用户
	IF @UseRange = 2
	BEGIN
		DECLARE @FILLCOUNT INT
		SELECT @FillCount=COUNT(USERID) FROM ShareSKDetailInfo WHERE UserID=@UserID
		IF @FillCount>0
		BEGIN
			SET @strErrorDescribe=N'抱歉！该会员卡只适合第一次充值的用户使用。'
			RETURN 302
		END
	END

	-- 房间锁定
	IF EXISTS (SELECT UserID FROM GameScoreLocker(NOLOCK) WHERE UserID=@UserID)
	BEGIN
		SET @strErrorDescribe='抱歉！您已经在金币游戏房间了，不能进行充值操作，请先退出金币游戏房间！'	
		RETURN 401
	END	

	--------------------------- 为用户绑定会员卡信息 -------------------------------
	-- 删除过期
	DELETE FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
	WHERE UserID=@UserID AND MemberOrder=@MemberOrder AND MemberOverDate<=GETDATE()

	-- 更新会员
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember SET MemberOverDate=MemberOverDate+@MemberDays
	WHERE UserID=@UserID AND MemberOrder=@MemberOrder

	IF @@ROWCOUNT=0
	BEGIN
		INSERT QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
		VALUES (@UserID,@MemberOrder,@UserRight,GETDATE()+@MemberDays)
	END

	-- 绑定会员,(会员期限与切换时间)
	SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember WHERE UserID=@UserID

	-- 会员权限
	SELECT @MaxUserRight=UserRight FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
	WHERE UserID=@UserID AND MemberOrder=@MaxMemberOrder
	
	-- 附加会员卡信息
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo
	SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
	WHERE UserID=@UserID

	-- 充值金币	
	DECLARE @CurScore BIGINT
	SELECT @CurScore=Score FROM GameScoreInfo WHERE UserID=@UserID
	IF @CurScore IS NULL
	BEGIN
		SET @CurScore=0
		INSERT GameScoreInfo(UserID,Score,RegisterIP,LastLogonIP)
		VALUES (@UserID,@CardGold,@strClientIP,@strClientIP)
	END
	ELSE
	BEGIN
		UPDATE GameScoreInfo SET Score=Score+@CardGold WHERE UserID=@UserID
	END
	--------------------------------------------------------------------------------

	-- 推广系统
	IF @SpreaderID<>0
	BEGIN
		DECLARE @Rate DECIMAL(18,2)
		DECLARE @GrantScore BIGINT
		DECLARE @Note NVARCHAR(512)
		SELECT @Rate=FillGrantRate FROM GlobalSpreadInfo
		IF @Rate IS NULL
		BEGIN
			SET @Rate=0.1
		END
		SET @GrantScore = @CardGold*@Rate
		SET @Note = N'充值'+LTRIM(STR(@CardPrice))+'元'
		INSERT INTO RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@GrantScore,3,@UserID,@Note)		
	END

	-- 设置卡已使用
	UPDATE LivcardAssociator SET ApplyDate=GETDATE() WHERE CardID=@CardID

	-- 写卡充值记录
	INSERT INTO ShareDetailInfo(
		OperUserID,ShareID,UserID,GameID,Accounts,SerialID,CardTypeID,CardPrice,CardGold,BeforeGold,CardTotal,PayAmount,IPAddress,ApplyDate)
	VALUES(@dwOperUserID,@ShareID,@UserID,@GameID,@Accounts,@SerialID,@CardTypeID,@CardPrice,@CardGold,@CurScore,1,@CardPrice,@strClientIP,GETDATE())

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)	
	
	UPDATE StreamShareInfo
	SET ShareTotals=ShareTotals+1
	WHERE DateID=@DateID AND ShareID=@ShareID

	IF @@ROWCOUNT=0
	BEGIN
		INSERT StreamShareInfo(DateID,ShareID,ShareTotals)
		VALUES (@DateID,@ShareID,1)
	END	 

	SET @strErrorDescribe=N'实卡充值成功。'
END 

RETURN 0
GO



