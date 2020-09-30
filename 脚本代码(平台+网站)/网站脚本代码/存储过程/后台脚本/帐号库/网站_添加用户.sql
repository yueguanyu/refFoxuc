----------------------------------------------------------------------
-- 时间：2011-09-29
-- 用途：后台管理员添加用户信息
----------------------------------------------------------------------
USE QPAccountsDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[NET_PM_AddAccount]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[NET_PM_AddAccount]
GO

----------------------------------------------------------------------
CREATE PROC [NET_PM_AddAccount]
(
	@strAccounts		NVARCHAR(31),			--用户帐号
	@strNickName		NVARCHAR(31)=N'',		--用户昵称
	@strLogonPass		NCHAR(32),				--登录密码
	@strInsurePass		NCHAR(32),				--安全密码
	@dwFaceID			SMALLINT ,				--头像
	@strUnderWrite			NVARCHAR(18)=N'',	--个性签名
	@strPassPortID		NVARCHAR(18)=N'',		--身份证号
	@strCompellation	NVARCHAR(16)=N'',		--真实名字	
	
	@dwExperience		INT	= 0,				--经验数值
	@dwPresent			INT	= 0,				--礼物数值
	@dwLoveLiness		INT	= 0,				--魅力值数	
	@dwUserRight		INT	= 0,				--用户权限
	@dwMasterRight		INT	= 0,				--管理权限
	@dwServiceRight		INT	= 0,				--服务权限
	@dwMasterOrder		TINYINT	= 0,			--管理等级
	
	@dwMemberOrder		TINYINT	= 0,			--会员等级
	@dtMemberOverDate	DATETIME='1980-01-01',	--过期日期
	@dtMemberSwitchDate DATETIME='1980-01-01',	--切换时间
	@dwGender			TINYINT = 1,			--用户性别
	@dwNullity			TINYINT = 0,			--禁止服务
	@dwStunDown			TINYINT = 0,			--关闭标志
	@dwMoorMachine		TINYINT = 0,			--固定机器	
	@strRegisterIP		NVARCHAR(15),			--注册地址
	@strRegisterMachine NVARCHAR(32)=N'',		--注册机器        
	@IsAndroid			TINYINT,
	                
	@strQQ				NVARCHAR(16)=N'',		--QQ 号码
	@strEMail			NVARCHAR(32)=N'',		--电子邮件
	@strSeatPhone		NVARCHAR(32)=N'',		--固定电话
	@strMobilePhone		NVARCHAR(16)=N'',		--手机号码
	@strDwellingPlace	NVARCHAR(128)=N'',		--详细住址
	@strPostalCode		NVARCHAR(8)=N'',		--邮政编码               
	@strUserNote		NVARCHAR(256)=N''		--用户备注
	
)
			
--WITH ENCRYPTION AS
AS
BEGIN
	-- 属性设置
	SET NOCOUNT ON

	DECLARE @dwUserID			INT,			-- 用户标识
			@GameID				INT,			-- 游戏ID
			@dtCurrentDate		DATETIME,
			@dwDefSpreaderScale DECIMAL(18,2)	--默认的抽水比例值0.10
	SET @dwDefSpreaderScale = 0.10
	SET @dtCurrentDate =  GETDATE()

	-- 执行逻辑
	BEGIN TRY
	--验证
	IF @strAccounts IS NULL OR @strAccounts = ''
		RETURN -2;	--参数有误
	IF @strNickName IS NULL OR @strNickName = ''
		SET @strNickName = @strAccounts
	IF EXISTS (SELECT * FROM AccountsInfo WHERE Accounts=@strAccounts OR RegAccounts=@strAccounts)
		RETURN -3;	--	帐号已存在
	IF EXISTS (SELECT * FROM AccountsInfo WHERE NickName=@strNickName)
		RETURN -4;	--	昵称已存在
	-- 效验帐号
	IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE (EnjoinOverDate IS NULL  OR EnjoinOverDate>=GETDATE()) AND CHARINDEX(String,@strAccounts)>0)
	BEGIN		
		RETURN -5;	-- 您所输入的帐号名含有限制字符串
	END
		
	-- 注册赠送金币	
	DECLARE @GrantScoreCount AS INT
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'
	
	IF @GrantScoreCount IS NULL OR @GrantScoreCount = '' OR @GrantScoreCount <= 0
		SET @GrantScoreCount = 0;
	
	IF @strNickName IS NULL OR 	@strNickName = ''
		SET @strNickName = @strAccounts
	IF @strInsurePass IS NULL OR @strInsurePass = ''
		SET @strInsurePass = @strLogonPass
	BEGIN TRAN
		--用户信息
		INSERT AccountsInfo
				( Accounts,NickName,RegAccounts,UnderWrite,PassPortID,
				  Compellation ,LogonPass ,InsurePass ,FaceID,Experience ,
				  Present,LoveLiness,UserRight ,MasterRight ,ServiceRight ,
				  MasterOrder ,MemberOrder ,MemberOverDate ,MemberSwitchDate ,Gender ,
				  Nullity ,StunDown ,MoorMachine ,LastLogonIP,RegisterIP ,
				  RegisterDate ,RegisterMobile ,RegisterMachine ,IsAndroid
				)
		VALUES  (				  			  
				  @strAccounts , -- Accounts - nvarchar(31)
				  @strNickName , -- NickName - nvarchar(31)
				  @strAccounts , -- RegAccounts - nvarchar(31)	
				  @strUnderWrite,			  
				  @strPassPortID , -- PassPortID - nvarchar(18)
				  
				  @strCompellation , -- Compellation - nvarchar(16)
				  @strLogonPass , -- LogonPass - nchar(32)
				  @strInsurePass , -- InsurePass - nchar(32)
				  @dwFaceID,
				  @dwExperience , -- Experience - int
				  
				  @dwPresent,							
				  @dwLoveLiness,
				  @dwUserRight , -- UserRight - int
				  @dwMasterRight , -- MasterRight - int
				  @dwServiceRight , -- ServiceRight - int
				  
				  @dwMasterOrder , -- MasterOrder - tinyint
				  @dwMemberOrder , -- MemberOrder - tinyint
				  @dtMemberOverDate , -- MemberOverDate - datetime
				  @dtMemberSwitchDate , -- MemberSwitchDate - datetime
				  @dwGender , -- Gender - tinyint
				  
				  @dwNullity, -- Nullity - tinyint
				  @dwStunDown , -- StunDown - tinyint
				  @dwMoorMachine , -- MoorMachine - tinyint	  
				  @strRegisterIP,      
				  @strRegisterIP , -- RegisterIP - nvarchar(15)
				  
				  @dtCurrentDate , -- RegisterDate - datetime
				  @strMobilePhone , -- RegisterMobile - nvarchar(11)
				  @strRegisterMachine , -- RegisterMachine - nvarchar(32)
				  @IsAndroid  -- IsAndroid - tinyint				 
				)
				--用户标识
		        SET @dwUserID  = @@IDENTITY
		       
				--用户详细信息
				INSERT IndividualDatum
						( UserID ,Compellation ,QQ ,EMail ,SeatPhone ,
						  MobilePhone ,DwellingPlace ,PostalCode ,CollectDate ,UserNote
						)
				VALUES  ( @dwUserID , -- UserID - int
						  @strCompellation , -- Compellation - nvarchar(16)
						  @strQQ , -- QQ - nvarchar(16)
						  @strEMail , -- EMail - nvarchar(32)
						  @strSeatPhone , -- SeatPhone - nvarchar(32)
						  @strMobilePhone , -- MobilePhone - nvarchar(16)
						  @strDwellingPlace , -- DwellingPlace - nvarchar(128)
						  @strPostalCode , -- PostalCode - nvarchar(8)
						  @dtCurrentDate , -- CollectDate - datetime
						  @strUserNote  -- UserNote - nvarchar(256)
						)
				-- 用户财富信息
				INSERT QPTreasureDB.dbo.GameScoreInfo
				        ( UserID ,	
				          Score,
				          Revenue,
				          InsureScore,		          
				          UserRight ,
				          MasterRight ,
				          MasterOrder ,
				          LastLogonMachine,				         
				          LastLogonIP ,				         
				          RegisterIP ,
				          RegisterDate,
				          RegisterMachine
				           
				        )
				VALUES  ( @dwUserID , -- UserID - int
				          @GrantScoreCount , -- Score - bigint
				          0 , -- Revenue - bigint
				          0 , -- InsureScore - bigint				        
				          @dwUserRight , -- UserRight - int
				          @dwMasterRight , -- MasterRight - int
				          @dwMasterOrder , -- MasterOrder - tinyint	
				          '',			          
				          @strRegisterIP , -- LastLogonIP - nvarchar(15)
				          @strRegisterIP , -- RegisterIP - nvarchar(15)
				          @dtCurrentDate,  -- RegisterDate - datetime
				          ''				          				         
				        )    
			
			-- 记录日志			
			UPDATE SystemStreamInfo SET WebRegisterSuccess=WebRegisterSuccess+1 WHERE DateID=@DateID
			IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, WebRegisterSuccess) VALUES (@DateID, 1)
			
			IF @GrantScoreCount > 0
			BEGIN 
				-- 更新赠送金币记录
				UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strRegisterIP

				-- 插入记录
				IF @@ROWCOUNT=0		
					INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strRegisterIP, '', @GrantScoreCount, 1)		
			END 
			
			-- 分配游戏ID
			SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@dwUserID
			IF @GameID IS NULL 
			BEGIN
				COMMIT TRAN			
				RETURN 1;--用户添加成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！				
			END
			ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@dwUserID  
			     
			COMMIT TRAN			
			RETURN 0;--成功
		END TRY
		BEGIN CATCH
			IF @@TRANCOUNT > 0
			BEGIN
				ROLLBACK TRAN
			END
			RETURN -1;    --未知服务器错误
		END CATCH
END
GO