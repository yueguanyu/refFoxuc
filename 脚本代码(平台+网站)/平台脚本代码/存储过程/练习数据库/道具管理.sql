
----------------------------------------------------------------------------------------------------

USE QPEducateDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadGameProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadGameProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ConsumeProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ConsumeProperty]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载道具
CREATE PROC GSP_GR_LoadGameProperty
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT							-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载道具
	SELECT ID, IssueArea, Cash, Gold, Discount, SendLoveLiness, RecvLoveLiness FROM GameProperty(NOLOCK)
	WHERE Nullity=0 AND (IssueArea&6)<>0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 消费道具
CREATE PROC GSP_GR_ConsumeProperty

	-- 消费信息
	@dwSourceUserID INT,						-- 用户标识
	@dwTargetUserID INT,						-- 用户标识
	@wPropertyID INT,							-- 道具标识
	@wPropertyCount INT,						-- 道具数目

	-- 消费区域
	@wKindID INT,								-- 游戏标识
	@wServerID INT,								-- 房间标识
	@wTableID INT,								-- 桌子标示

	-- 购买方式
	@cbConsumeScore BIT,						-- 积分消费
	@lFrozenedScore BIGINT,						-- 冻结积分

	-- 系统信息
	@dwEnterID INT,								-- 进入索引
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询会员
	DECLARE @Nullity BIT
	DECLARE @CurrMemberOrder SMALLINT	
	SELECT @Nullity=Nullity, @CurrMemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwSourceUserID

	-- 用户判断
	IF @CurrMemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在，请联系客户服务中心了解详细情况！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END

	-- 目标用户
	IF @dwTargetUserID<>0 AND NOT EXISTS( SELECT UserID FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwTargetUserID )
	BEGIN
		SET @strErrorDescribe=N'您所赠送的目标用户不存在，请查证后再次尝试！'
		RETURN 3
	END

	-- 变量定义
	DECLARE @Gold AS BIGINT
	DECLARE @Discount AS SMALLINT
	DECLARE @SendLoveLiness AS BIGINT
	DECLARE @RecvLoveLiness AS BIGINT
	DECLARE @EffectRows AS INT

	-- 道具判断
	SELECT @Gold=Gold, @Discount=Discount, @SendLoveLiness=SendLoveLiness, @RecvLoveLiness=RecvLoveLiness
	FROM GameProperty(NOLOCK) WHERE Nullity=0 AND ID=@wPropertyID

	-- 存在判断
	IF @SendLoveLiness IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所购买的道具礼物不存在或者正在维护中，请联系客户服务中心了解详细情况！'
		RETURN 4
	END

	-- 费用计算
	DECLARE @ConsumeGold BIGINT
	IF @CurrMemberOrder=0 SELECT @ConsumeGold=@Gold*@wPropertyCount
	ELSE SELECT @ConsumeGold=@Gold*@wPropertyCount*@Discount/100

	-- 银行扣费
	IF @cbConsumeScore=0
	BEGIN
		-- 读取银行
		DECLARE @Insure BIGINT
		SELECT @Insure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwSourceUserID
	
		-- 银行判断
		IF @Insure<@ConsumeGold
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'您的保险柜游戏币余额不足，请选择其他的购买方式或者往保险柜存入足够的游戏币后再次尝试！'
			RETURN 5
		END

		-- 银行扣费
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@ConsumeGold WHERE UserID=@dwSourceUserID	
	END
	ELSE
	BEGIN

		-- 查询锁定
		DECLARE @EnterID INT
		DECLARE @LockKindID INT
		DECLARE @LockServerID INT
		SELECT @LockKindID=KindID, @LockServerID=ServerID, @EnterID=EnterID FROM GameScoreLocker WHERE UserID=@dwSourceUserID

		-- 锁定判断
		IF @LockKindID<>@wKindID OR @LockServerID<>@wServerID OR @dwEnterID<>@EnterID
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'您的房间锁定记录信息效验失败，请联系客户服务中心了解详细情况！'
			RETURN 5
		END

		-- 冻结判断
		IF @lFrozenedScore<@ConsumeGold
		BEGIN
			-- 错误信息
			SET @strErrorDescribe=N'您当前的游戏币余额不足，请选择其他的购买方式或者充值后再次尝试！'
			RETURN 5
		END

	END
	
	-- 负分清零
	IF @wPropertyID=16
	BEGIN
		UPDATE GameScoreInfo SET Score=0 WHERE UserID=@dwTargetUserID			
	END

	-- 会员卡
	IF @wPropertyID >= 22 AND @wPropertyID <= 26
	BEGIN
		-- 会员资料
		DECLARE @MemberOrder		 INT
		DECLARE @MemberRight		 INT
		DECLARE @MemberValidMonth	 INT			
		
		DECLARE @MaxMemberOrder INT
		DECLARE @MaxUserRight INT
		DECLARE @MemberOverDate DATETIME
		DECLARE @MemberSwitchDate DATETIME

		-- 设置变量
		SET @MemberOrder=@wPropertyID-22+1
		SET @MemberRight=512

		-- 有效期限
		SELECT @MemberValidMonth= @wPropertyCount * 1

		-- 删除过期
		DELETE FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
		WHERE UserID=@dwTargetUserID AND MemberOrder=@MemberOrder AND MemberOverDate<=GETDATE()

		-- 更新会员
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember SET MemberOverDate=DATEADD(mm,@MemberValidMonth, GETDATE())
		WHERE UserID=@dwTargetUserID AND MemberOrder=@MemberOrder
		IF @@ROWCOUNT=0
		BEGIN
			INSERT QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
			VALUES (@dwTargetUserID,@MemberOrder,@MemberRight,DATEADD(mm,@MemberValidMonth, GETDATE()))
		END

		-- 绑定会员,(会员期限与切换时间)
		SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
		FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember WHERE UserID=@dwTargetUserID

		-- 会员权限
		SELECT @MaxUserRight=UserRight FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
		WHERE UserID=@dwTargetUserID AND MemberOrder=@MaxMemberOrder
		
		-- 附加会员卡信息
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo
		SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
		WHERE UserID=@dwTargetUserID
	END

	-- 会员等级
	SELECT @CurrMemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwTargetUserID

	-- 购买记录
	IF @wPropertyID<=13 -- 礼物
	BEGIN
		INSERT QPAccountsDBLink.QPRecordDB.dbo.RecordSendPresent(PresentID,RcvUserID,SendUserID,LovelinessRcv,LovelinessSend,PresentPrice,PresentCount,KindID,ServerID,SendTime,ClientIP)
		VALUES(@wPropertyID,@dwTargetUserID,@dwSourceUserID,@RecvLoveLiness,@SendLoveLiness,@Gold,@wPropertyCount,@wKindID,@wServerID,GETDATE(),@strClientIP)
	END
	ELSE
	BEGIN -- 道具
		INSERT QPAccountsDBLink.QPRecordDB.dbo.RecordSendProperty(PropID,SourceUserID,TargetUserID,PropPrice,PropCount,KindID,ServerID,SendTime,ClientIP)
		VALUES(@wPropertyID,@dwSourceUserID,@dwTargetUserID,@Gold,@wPropertyCount,@wKindID,@wServerID,GETDATE(),@strClientIP)		
	END

	-- 输出记录
	SELECT @ConsumeGold AS ConsumeGold, @SendLoveLiness*@wPropertyCount AS SendLoveLiness, @RecvLoveLiness*@wPropertyCount AS RecvLoveLiness,@CurrMemberOrder AS MemberOrder

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------