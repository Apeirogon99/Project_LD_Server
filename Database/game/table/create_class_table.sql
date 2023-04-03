USE game_database

DROP TABLE IF EXISTS character_class_tb;
GO

CREATE TABLE character_class_tb
(
	id					TINYINT NOT NULL PRIMARY KEY,
	name				NVARCHAR(24) NOT NULL,
)
GO

INSERT INTO character_class_tb(id, name) VALUES (0, '미확인')
INSERT INTO character_class_tb(id, name) VALUES (1, '전사')
INSERT INTO character_class_tb(id, name) VALUES (2, '마법사')
GO

SELECT * FROM character_class_tb
Go