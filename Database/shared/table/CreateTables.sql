USE shared_database

ALTER DATABASE shared_database SET SINGLE_USER WITH ROLLBACK IMMEDIATE
Go

ALTER DATABASE shared_database COLLATE Korean_Wansung_CI_AS
Go

ALTER DATABASE shared_database  SET MULTI_USER
Go

DROP TABLE IF EXISTS token_tb;
DROP TABLE IF EXISTS server_tb;

CREATE TABLE token_tb
(
	global_id		INT PRIMARY KEY,
	token			CHAR(36) NOT NULL
);
GO

CREATE TABLE server_tb
(
	id		INT IDENTITY(1, 1) NOT NULL PRIMARY KEY,
	ip		VARCHAR(255) NOT NULL,
	port	INT NOT NULL,
);
GO

SELECT * FROM token_tb
SELECT * FROM server_tb	
GO;