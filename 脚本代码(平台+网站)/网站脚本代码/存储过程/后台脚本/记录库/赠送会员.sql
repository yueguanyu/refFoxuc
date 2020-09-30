----------------------------------------------------------------------
-- 版权：2009
-- 时间：2010-03-16
-- 用途：赠送会员
----------------------------------------------------------------------------------------------------

USE QPRecordDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[WSP_PM_GrantMember]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[WSP_PM_GrantMember]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO
----------------------------------------------------------------------
CREATE PROCEDURE WSP_PM_GrantMember
	@MasterID INT,					-- 管理员标识
	@ClientIP VARCHAR(15),			-- 赠送地址
	@UserID INT,					-- 用户标识
	@MemberOrder INT,				-- 赠送会员标识
	@Reason NVARCHAR(32),			-- 赠送原因
	@MemberDays INT					-- 赠送会员天数
	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 会员资料
DECLARE @MaxMemberOrder INT
DECLARE @MaxUserRight INT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- 执行逻辑
BEGIN
	
	-- 新增记录信息
	INSERT INTO	RecordGrantMember(MasterID,ClientIP,UserID,GrantCardType,Reason,MemberDays)
	VALUES(@MasterID,@ClientIP,@UserID,@MemberOrder,@Reason,@MemberDays)

	
	-- 更新会员
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember SET MemberOverDate = MemberOverDate + @MemberDays
	WHERE UserID = @UserID AND MemberOrder = @MemberOrder
	
	IF @@ROWCOUNT=0
	BEGIN
		INSERT INTO QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
		VALUES(@UserID,@MemberOrder,512,GETDATE()+@MemberDays)
	END

	-- 绑定会员,(会员期限与切换时间)
	SELECT @MaxMemberOrder = MAX(MemberOrder),@MemberOverDate = MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember WHERE UserID = @UserID
	
	-- 会员权限
	SELECT @MaxUserRight = UserRight FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
	WHERE UserID = @UserID AND MemberOrder = @MaxMemberOrder

	-- 附加会员卡信息
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo 
	SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
	WHERE UserID = @UserID

	
END
RETURN 0
	