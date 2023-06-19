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
	id		INT IDENTITY(1, 1)	NOT NULL PRIMARY KEY,
	type	INT					NOT NULL,
	ip		VARCHAR(255)		NOT NULL,
	port	INT					NOT NULL,
	name	NVARCHAR(30)		NOT NULL,
);
GO

INSERT INTO server_tb (type, ip, port, name) VALUES (1, '116.41.116.247', 9000 , '인증서버1')
INSERT INTO server_tb (type, ip, port, name) VALUES (2, '116.41.116.247', 10000, '타론')
GO

SELECT * FROM token_tb
SELECT * FROM server_tb	
GO;