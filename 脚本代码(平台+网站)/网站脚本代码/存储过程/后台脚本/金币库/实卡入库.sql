USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_PM_LivcardAdd]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WSP_PM_LivcardAdd]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------
-- 时间：2010-03-16
-- 用途：实卡入库
----------------------------------------------------------------------
CREATE PROCEDURE WSP_PM_LivcardAdd
	@SerialID nvarchar(31),		-- 实卡卡号
	@Password nchar(32),		-- 实卡密码
	@BuildID int,				-- 生成标识
	@CardTypeID int,			-- 实卡类型
	@CardPrice decimal(18,2),	-- 实卡价格	
	@CardGold bigint,			-- 实卡金币
	@MemberOrder tinyint,		-- 会员等级
	@MemberDays int,			-- 会员天数
	@UserRight int,				-- 用户权限
	@ServiceRight int,			-- 服务权限
	@UseRange int,				-- 使用范围
	@SalesPerson nvarchar(31),	-- 销售商
	@ValidDate datetime			-- 有效日期	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	INSERT INTO LivcardAssociator(
		SerialID,Password,BuildID,CardTypeID,CardPrice,CardGold,MemberOrder,
		MemberDays,UserRight,ServiceRight,UseRange,SalesPerson,ValidDate)
	VALUES(
		@SerialID,@Password,@BuildID,@CardTypeID,@CardPrice,@CardGold,@MemberOrder,
		@MemberDays,@UserRight,@ServiceRight,@UseRange,@SalesPerson,@ValidDate)
END
RETURN 0

