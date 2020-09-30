----------------------------------------------------------------------
-- 版权：2011
-- 时间：2011-09-1
-- 用途：金币转账
----------------------------------------------------------------------

USE [QPTreasureDB]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_InsureTransfer') AND OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_InsureTransfer
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

--------------------------------------------------------------------------------
-- 存款
CREATE PROCEDURE NET_PW_InsureTransfer
	@dwSrcUserID		INT,						-- 源用户标识

	@dwDstUserID		INT,						-- 目标用户标识
	@strInsurePass		NVARCHAR(32),				-- 银行密码
	@dwSwapScore		BIGINT,						-- 转账金额
	@dwMinSwapScore		BIGINT,						-- 最小转账数目

	@strClientIP		NVARCHAR(15),				-- 操作地址
	@strCollectNote		NVARCHAR(63),				-- 备注
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 汇款用户
DECLARE @SrcUserID			INT
DECLARE @SrcInsurePass		NCHAR(32)
DECLARE @SrcNullity			TINYINT
DECLARE @SrcStunDown		TINYINT

-- 收款用户
DECLARE @DstUserID			INT
DECLARE @DstNullity			TINYINT
DECLARE @DstStunDown		TINYINT

-- 汇款用户金币信息
DECLARE @SrcScore			BIGINT		-- 汇款人现金		
DECLARE @SrcInsureScore		BIGINT		-- 汇款人银行
DECLARE @SrcInsureBalance	BIGINT		-- 汇款人余额	

-- 收款用户金币信息
DECLARE @DstScore			BIGINT		-- 收款人现金			
DECLARE @DstInsureScore		BIGINT		-- 收款人银行
DECLARE @DstInsureBalance	BIGINT		-- 收款人余额

-- 税收
DECLARE @Revenue BIGINT
DECLARE @InsureRevenue BIGINT

-- 执行逻辑
BEGIN
	DECLARE @EnjoinInsure INT
	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 1
	END

	-- 判断是否自己转给自己
	IF @dwSrcUserID = @dwDstUserID
	BEGIN
		SET @strErrorDescribe=N'抱歉的通知您！同一帐号不允许进行赠送金币。'
		RETURN 1
	END
	
	--------------------------------------------------------------------------------
	/* 汇款人部分 */

	-- 查询用户	
	SELECT @SrcUserID=UserID,@SrcNullity=Nullity, @SrcStunDown=StunDown,@SrcInsurePass=InsurePass	
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwSrcUserID

	-- 验证用户
	IF @SrcUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 101
	END

	-- 帐号封停
	IF @SrcNullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 102
	END	

	-- 帐号关闭
	IF @SrcStunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！'
		RETURN 103
	END

	-- 银行密码
	IF @SrcInsurePass <> @strInsurePass 
	BEGIN
		SET @strErrorDescribe=N'您的银行密码输入有误，请查证后再次尝试！'	
		RETURN 104
	END	
	--------------------------------------------------------------------------------
	
	--------------------------------------------------------------------------------
	/* 收款人部分 */

	-- 查询用户	
	SELECT @DstUserID=UserID,@DstNullity=Nullity,@DstStunDown=StunDown
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwDstUserID

	-- 验证用户
	IF @DstUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'抱歉！您要赠送的目标帐号不存在！'
		RETURN 201
	END

	-- 帐号封停
	IF @DstNullity<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉！您赠送的目标帐号暂时处于冻结状态，无法进行金币赠送！'
		RETURN 202
	END	

	-- 帐号关闭
	IF @DstStunDown<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉！您赠送的目标帐号使用了安全关闭功能，无法进行金币赠送！'
		RETURN 203
	END
	--------------------------------------------------------------------------------

	-- 房间锁定
	IF EXISTS (SELECT UserID FROM GameScoreLocker(NOLOCK) WHERE UserID=@dwSrcUserID)
	BEGIN
		SET @strErrorDescribe='您已经在金币游戏房间了，需要进行转账操作，请先退出金币游戏房间！'		
		RETURN 2
	END	
	
	-- 最小转账金额	
	IF @dwSwapScore < @dwMinSwapScore
	BEGIN
		SET @strErrorDescribe=N'非常抱歉,您每笔保管的数目必须大于' + Convert(NVARCHAR(30), @dwMinSwapScore) + '金币！'
		RETURN 3
	END
	
	--------------------------------------------------------------------------------
	/* 汇款人部分 */

	-- 金币查询
	SELECT @SrcScore= Score, @SrcInsureScore=InsureScore
	FROM GameScoreInfo WHERE UserID=@dwSrcUserID
	
	-- 金币负数判断
	IF @SrcInsureScore IS NULL OR @SrcInsureScore<0
	BEGIN
		SET @strErrorDescribe=N'非常抱歉,您银行的保管余额不足!'
		RETURN 4
	END
	--银行转账条件，银行金币数必须大于此数才可转账！
	DECLARE @TransferPrerequisite INT; SET @TransferPrerequisite = 0
	SELECT @TransferPrerequisite=ISNULL(StatusValue,0) FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	IF @SrcInsureScore <= @TransferPrerequisite
	BEGIN
		SET @strErrorDescribe=N'非常抱歉,您的银行金币数必须大于'+LTRIM(@TransferPrerequisite)+'才可转账'
		RETURN 5
	END
	-- 转账税收
	DECLARE @RevenueRate INT
	SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'

	-- 税收调整
	IF @RevenueRate>300 SET @RevenueRate=300
	IF @RevenueRate IS NULL SET @RevenueRate=1	

	-- 金币判断
	IF @dwSwapScore > @SrcInsureScore
	BEGIN
		SET @strErrorDescribe=N'非常抱歉,您银行的保管余额不足!'
		RETURN 5
	END
	
	-- 余额计算
	SET @Revenue=@dwSwapScore*@RevenueRate/1000
	SET @SrcInsureBalance = @SrcInsureScore - @dwSwapScore
	--------------------------------------------------------------------------------
	
	--------------------------------------------------------------------------------
	/* 收款人部分 */
	
	-- 金币查询
	SELECT @DstScore= Score, @DstInsureScore=InsureScore
	FROM GameScoreInfo WHERE UserID=@dwDstUserID

	-- 验证用户
	IF @DstScore IS NULL OR @DstInsureScore IS NULL
	BEGIN
		-- 初始用户
		SET @DstScore = 0
		SET @DstInsureScore = 0

		INSERT INTO GameScoreInfo(UserID,LastLogonIP,RegisterIP)
		VALUES(@dwDstUserID,@strClientIP,@strClientIP)
	END

	-- 余额计算
	SET @DstInsureBalance = @DstInsureScore + @dwSwapScore - @Revenue
	--------------------------------------------------------------------------------

	-- 转账记录
	INSERT INTO RecordInsure(
		SourceUserID,SourceGold,SourceBank,TargetUserID,TargetGold,TargetBank,SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP,CollectNote)
	VALUES(@SrcUserID,@SrcScore,@SrcInsureScore,@DstUserID,@DstScore,@DstInsureScore,@dwSwapScore,@Revenue,1,3,@strClientIP,@strCollectNote)

	-- 转账操作
	-- 收款用户
	UPDATE GameScoreInfo SET InsureScore=@DstInsureBalance
	WHERE UserID=@dwDstUserID

	-- 汇款用户	
	UPDATE GameScoreInfo SET InsureScore=@SrcInsureBalance,Revenue=Revenue+@Revenue
	WHERE UserID=@dwSrcUserID

	SET @strErrorDescribe=N'转账成功!'
	
END

SET NOCOUNT OFF

RETURN 0
GO



