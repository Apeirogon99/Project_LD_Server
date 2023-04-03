USE shared_database

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