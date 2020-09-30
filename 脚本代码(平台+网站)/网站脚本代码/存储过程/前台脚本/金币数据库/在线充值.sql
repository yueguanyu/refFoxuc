----------------------------------------------------------------------
-- 版权：2011
-- 时间：2011-09-1
-- 用途：在线充值
----------------------------------------------------------------------

USE [QPTreasureDB]
GO

-- 在线充值
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_FilledOnLine') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_FilledOnLine
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

---------------------------------------------------------------------------------------
-- 在线充值
CREATE PROCEDURE NET_PW_FilledOnLine
	@strOrdersID		NVARCHAR(50),			--	订单编号
	@strOrderAmount		decimal,				--  实际支付金额
	@isVB				INT,					--	是否电话充值
	@strIPAddress		NVARCHAR(31),			--	用户帐号	
	@strErrorDescribe	NVARCHAR(127) OUTPUT	--	输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 订单信息
DECLARE @OperUserID INT
DECLARE @ShareID INT
DECLARE @UserID INT
DECLARE @GameID INT
DECLARE @Accounts NVARCHAR(31)
DECLARE @OrderID NVARCHAR(32)
DECLARE @CardTypeID INT
DECLARE @CardPrice DECIMAL(18,2)
DECLARE @CardGold BIGINT
DECLARE @CardTotal INT
DECLARE @OrderAmount DECIMAL(18,2)
DECLARE @DiscountScale DECIMAL(18,2)
DECLARE @PayAmount DECIMAL(18,2)
DECLARE @IPAddress NVARCHAR(15)

-- 会员卡信息
DECLARE @MemberOrder TINYINT
DECLARE @MemberDays INT
DECLARE @UserRight INT

-- 会员资料
DECLARE @MaxMemberOrder INT
DECLARE @MaxUserRight INT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 金币信息
DECLARE @Score BIGINT

-- 执行逻辑
BEGIN
	-- 订单查询
	SELECT @OperUserID=OperUserID,@ShareID=ShareID,@UserID=UserID,@GameID=GameID,@Accounts=Accounts,
		@OrderID=OrderID,@CardTypeID=CardTypeID,@CardPrice=CardPrice,@CardGold=CardGold,@CardTotal=CardTotal,
		@OrderAmount=OrderAmount,@DiscountScale=DiscountScale,@PayAmount=PayAmount
	FROM OnLineOrder WHERE OrderID=@strOrdersID

	if @strOrderAmount<>@OrderAmount
	BEGIN
		SET @strErrorDescribe=N'非法操作！提交的支付金额与实际支付金额不符'
		RETURN 1
	END	

	-- 订单存在
	IF @OrderID IS NULL 
	BEGIN
		SET @strErrorDescribe=N'抱歉！充值订单不存在。'
		RETURN 1
	END

	-- 订单重复
	IF EXISTS(SELECT OrderID FROM ShareDetailInfo(NOLOCK) WHERE OrderID=@strOrdersID) 
	BEGIN
		SET @strErrorDescribe=N'抱歉！充值订单重复。'
		RETURN 2
	END

	-- 金币查询
	SELECT @Score=Score FROM GameScoreInfo WHERE UserID=@UserID

	-- 产生记录
	INSERT INTO ShareDetailInfo(
		OperUserID,ShareID,UserID,GameID,Accounts,OrderID,CardTypeID,CardPrice,CardGold,BeforeGold,
		CardTotal,OrderAmount,DiscountScale,PayAmount,IPAddress)
	VALUES(
		@OperUserID,@ShareID,@UserID,@GameID,@Accounts,@OrderID,@CardTypeID,@CardPrice,@CardGold,@Score,
		@CardTotal,@OrderAmount,@DiscountScale,@PayAmount,@strIPAddress)


	-- 会员卡查询
	SELECT @MemberOrder=MemberOrder,@MemberDays=MemberDays,@UserRight=UserRight
	FROM GlobalLivcard WHERE CardTypeID=@CardTypeID

	SET @MemberDays = @MemberDays * @OrderAmount / @CardPrice

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
	SET @CardGold = @CardGold*@OrderAmount/@CardPrice

	IF @isVB = 1
	BEGIN
		SET @CardGold = @CardGold/2
	END

	UPDATE GameScoreInfo SET Score=Score+@CardGold WHERE UserID=@UserID
	IF @@ROWCOUNT=0	
	BEGIN
		INSERT GameScoreInfo(UserID,Score,RegisterIP,LastLogonIP)
		VALUES (@UserID,@CardGold,@strIPAddress,@strIPAddress)
	END
	--------------------------------------------------------------------------------

	-- 推广系统
	DECLARE @SpreaderID INT	
	SELECT @UserID=UserID,@Accounts=Accounts,@SpreaderID=SpreaderID FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo
	WHERE UserID = @UserID
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

	-- 更新订单状态
	UPDATE OnLineOrder SET OrderStatus=2 WHERE OrderID=@OrderID

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
	
END 
RETURN 0
GO



