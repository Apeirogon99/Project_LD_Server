USE game_database

DROP TABLE IF EXISTS race_tb;
GO

CREATE TABLE race_tb
(
	id					TINYINT NOT NULL PRIMARY KEY,
	name				NVARCHAR(24) NOT NULL,
)
GO

INSERT INTO race_tb(id, name) VALUES (0, '��Ȯ��')
INSERT INTO race_tb(id, name) VALUES (1, '����')
INSERT INTO race_tb(id, name) VALUES (2, '����')
INSERT INTO race_tb(id, name) VALUES (3, '��ũ')
GO

SELECT * FROM race_tb
Go