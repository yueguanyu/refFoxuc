
USE master
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPNativeWebDB')
DROP DATABASE [QPNativeWebDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPPlatformManagerDB')
DROP DATABASE [QPPlatformManagerDB]
GO



GO
