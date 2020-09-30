----------------------------------------------------------------------
-- 时间：2011-10-20
-- 用途：数据汇总统计。
----------------------------------------------------------------------
USE QPTreasureDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PM_StatInfo') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PM_StatInfo
GO

----------------------------------------------------------------------
CREATE PROC NET_PM_StatInfo
			
WITH ENCRYPTION AS

BEGIN
	-- 属性设置
	SET NOCOUNT ON;	
	--用户统计
	DECLARE @OnLineCount INT		--在线人数
	DECLARE @DisenableCount INT		--停权用户
	DECLARE @AllCount INT			--注册总人数
	SELECT  TOP 1 @OnLineCount=ISNULL(OnLineCountSum,0) FROM  QPPlatformDB.dbo.OnLineStreamInfo(NOLOCK) ORDER BY InsertDateTime DESC
	SELECT  @DisenableCount=COUNT(UserID) FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE Nullity = 1
	SELECT  @AllCount=COUNT(UserID) FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE IsAndroid=0

	--金币统计
	DECLARE @Score BIGINT		--金币总量
	DECLARE @InsureScore BIGINT	--银行总量
	DECLARE @AllScore BIGINT
	SELECT  @Score=SUM(Score),@InsureScore=SUM(InsureScore),@AllScore=SUM(Score+InsureScore) FROM QPTreasureDB.dbo.GameScoreInfo(NOLOCK)

	--赠送统计
	DECLARE @RegGrantScore BIGINT		--注册赠送
	DECLARE @PresentScore BIGINT		--泡分赠送
	DECLARE @ManagerGrantScore BIGINT	--管理员后台手动赠送
	SELECT  @RegGrantScore=SUM(GrantScore) FROM QPAccountsDB.dbo.SystemGrantCount(NOLOCK)
	SELECT  @PresentScore=ISNULL(SUM(PresentScore),0) FROM QPTreasureDB.dbo.StreamPlayPresent(NOLOCK)
	SELECT @ManagerGrantScore=ISNULL(SUM(CONVERT(BIGINT,AddGold)),0) FROM QPRecordDB.dbo.RecordGrantTreasure(NOLOCK)
	
	--魅力统计
	DECLARE @LoveLiness INT		--魅力总量
	DECLARE @Present INT		--已兑换魅力总量
	DECLARE @ConvertPresent BIGINT --已兑换金币量
	SELECT  @LoveLiness=SUM(LoveLiness),@Present=SUM(Present) FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK)
	SELECT  @ConvertPresent=SUM(CONVERT(BIGINT,ConvertPresent)*ConvertRate) FROM QPRecordDB.dbo.RecordConvertPresent(NOLOCK)

	--税收统计
	DECLARE @Revenue BIGINT			--税收总量
	DECLARE @TransferRevenue BIGINT	--转账税收
	SELECT @Revenue=SUM(Revenue) FROM QPTreasureDB.dbo.GameScoreInfo(NOLOCK)
	SELECT @TransferRevenue=SUM(Revenue) FROM QPTreasureDB.dbo.RecordInsure(NOLOCK)
/*
	--游戏税收：
	SELECT KindID,SUM(Revenue) FROM RecordUserInout(NOLOCK) GROUP BY KindID
	--房间税收
	SELECT ServerID,SUM(Revenue) FROM RecordUserInout(NOLOCK) GROUP BY ServerID
*/
	--损耗统计
	DECLARE @Waste FLOAT   --损耗总量
	SELECT @Waste=SUM(Waste) FROM QPTreasureDB.dbo.RecordDrawInfo(NOLOCK)
	--SELECT * FROM QPTreasureDB.dbo.RecordDrawInfo(NOLOCK)
	/*--游戏损耗
	SELECT KindID,SUM(Waste) FROM RecordDrawInfo(NOLOCK) GROUP BY KindID
	--房间损耗
	SELECT ServerID,SUM(Waste) FROM RecordDrawInfo(NOLOCK) GROUP BY ServerID
*/
	--点卡统计
	DECLARE @CardCount INT			--生成张数
	DECLARE @CardGold BIGINT			--金币总量
	DECLARE @CardPrice DECIMAL(18,2)--面额总量
	SELECT  @CardCount=COUNT(CardID),@CardGold=SUM(CardGold),@CardPrice=SUM(CardPrice) FROM QPTreasureDB.dbo.LivcardAssociator(NOLOCK)

	DECLARE @CardPayCount INT 		--充值张数
	DECLARE @CardPayGold INT		--充值金币
	DECLARE @CardPayPrice DECIMAL(18,2)--充值人民币总数
	SELECT @CardPayCount=COUNT(CardID),@CardPayGold=SUM(CardGold),@CardPayPrice=SUM(CardPrice) FROM QPTreasureDB.dbo.LivcardAssociator(NOLOCK) WHERE ApplyDate IS NOT NULL 

	DECLARE @MemberCardCount INT	--会员卡张数
	SELECT @MemberCardCount=COUNT(CardID) FROM QPTreasureDB.dbo.LivcardAssociator(NOLOCK) WHERE MemberOrder<>0

	--返回
	SELECT  @OnLineCount AS	OnLineCount,				--在线人数
			@DisenableCount AS DisenableCount,			--停权用户
			@AllCount AS AllCount,						--注册总人数
			@Score AS Score,							--金币总量
			@InsureScore AS InsureScore,				--银行总量
			@AllScore AS AllScore,
			@RegGrantScore AS RegGrantScore,			--注册赠送
			@PresentScore AS PresentScore,				--泡分赠送
			@ManagerGrantScore AS ManagerGrantScore,	--管理员后台手动赠送
			@LoveLiness AS LoveLiness,					--魅力总量
			@Present AS Present,						--已兑换魅力总量
			(@LoveLiness-@Present) AS RemainLove,		--未兑换魅力总量
			@ConvertPresent AS ConvertPresent,			--已兑换金币量
			@Revenue AS Revenue,						--税收总量
			@TransferRevenue AS TransferRevenue,		--转账税收	
			@Waste AS Waste,							--损耗总量
	
			@CardCount AS CardCount,					--生成张数
			@CardGold AS CardGold,						--金币总量
			@CardPrice AS CardPrice,					--面额总量
			@CardPayCount AS CardPayCount, 				--充值张数
			@CardPayGold AS CardPayGold,				--充值金币
			@CardPayPrice AS CardPayPrice,				--充值人民币总数
			@MemberCardCount AS MemberCardCount			--会员卡张数
END































