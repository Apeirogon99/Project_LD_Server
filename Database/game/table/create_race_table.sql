USE game_database

DROP TABLE IF EXISTS race_tb;
GO

CREATE TABLE race_tb
(
	id					TINYINT NOT NULL PRIMARY KEY,
	name				NVARCHAR(24) NOT NULL,
)
GO

INSERT INTO race_tb(id, name) VALUES (0, '미확인')
INSERT INTO race_tb(id, name) VALUES (1, '남자')
INSERT INTO race_tb(id, name) VALUES (2, '여자')
INSERT INTO race_tb(id, name) VALUES (3, '오크')
GO

SELECT * FROM race_tb
Go