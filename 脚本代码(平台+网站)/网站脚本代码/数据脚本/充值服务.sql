USE QPTreasureDB
GO

-- 充值服务
TRUNCATE TABLE GlobalShareInfo
GO

INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(1,'实卡充值','SK','实卡充值服务')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(2,'快钱充值','KQ','充值服务')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(3,'易宝充值','YB','易宝充值服务')

INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(4,'神州行充值','SZX','神州行服务')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(5,'联通卡充值','LT','联通卡服务')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(6,'电信卡充值','DX','电信卡充值')

INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(7,'盛大卡充值','SD','盛大卡充值')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(8,'征途卡充值','ZT','征途卡充值')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(9,'网易卡充值','WY','网易卡充值')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(10,'搜狐卡充值','SH','搜狐卡充值')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(11,'完美卡充值','WM','完美卡充值')
INSERT INTO GlobalShareInfo(ShareID,ShareName,ShareAlias,ShareNote) VALUES(12,'声讯充值','SX','声讯充值')

GO