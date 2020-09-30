USE QPTreasureDB
GO

-- 会员卡类型
TRUNCATE TABLE GlobalLivcard
GO

INSERT [dbo].[GlobalLivcard] ( [CardName], [CardPrice], [CardGold], [MemberOrder], [MemberDays], [UserRight], [ServiceRight]) VALUES ( N'水晶月卡', CAST(10.00 AS Decimal(18, 2)), 100000, 1, 30, 512, 0)
INSERT [dbo].[GlobalLivcard] ( [CardName], [CardPrice], [CardGold], [MemberOrder], [MemberDays], [UserRight], [ServiceRight]) VALUES ( N'黄金季卡', CAST(30.00 AS Decimal(18, 2)), 300000, 2, 90, 512, 0)
INSERT [dbo].[GlobalLivcard] ( [CardName], [CardPrice], [CardGold], [MemberOrder], [MemberDays], [UserRight], [ServiceRight]) VALUES ( N'白金半年卡', CAST(60.00 AS Decimal(18, 2)), 600000, 3, 180, 512, 0)
INSERT [dbo].[GlobalLivcard] ( [CardName], [CardPrice], [CardGold], [MemberOrder], [MemberDays], [UserRight], [ServiceRight]) VALUES ( N'钻石年卡', CAST(120.00 AS Decimal(18, 2)), 1200000, 4, 360, 512, 0)

GO