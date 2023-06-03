USE game_database

DROP TABLE IF EXISTS item_tb;
DROP TABLE IF EXISTS character_class_tb;
DROP TABLE IF EXISTS race_tb;
DROP TABLE IF EXISTS category_tb;
DROP TABLE IF EXISTS tier_tb;
GO

USE game_database
CREATE TABLE category_tb
(
	id					INT NOT NULL IDENTITY(1,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

BULK INSERT category_tb FROM 'P:\Project_LD_Server\Data\category_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
SELECT * FROM category_tb
GO

USE game_database
CREATE TABLE item_tb
(
	id					INT NOT NULL IDENTITY(1,1) PRIMARY KEY,
	category_id			INT NULL DEFAULT 0,
	character_class_id	INT NULL DEFAULT 0,
	race_id				INT NULL DEFAULT 0,
	tier_id				INT NULL DEFAULT 0,
	name				NVARCHAR(32) NULL DEFAULT '',
	description			NVARCHAR(32) NULL DEFAULT '',
	cost				INT NULL DEFAULT 0,
	level				INT NULL DEFAULT 0,
	size_x				INT NULL DEFAULT 0,
	size_y				INT NULL DEFAULT 0,
	icon				CHAR(256) NULL DEFAULT '',
	mesh				CHAR(256) NULL DEFAULT ''
)
BULK INSERT item_tb FROM 'P:\Project_LD_Server\Data\item_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
SELECT * FROM item_tb
GO

USE game_database
CREATE TABLE character_class_tb
(
	id					INT NOT NULL IDENTITY(1,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

BULK INSERT character_class_tb FROM 'P:\Project_LD_Server\Data\character_class_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
SELECT * FROM character_class_tb
GO

USE game_database
CREATE TABLE race_tb
(
	id					INT NOT NULL IDENTITY(1,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

BULK INSERT race_tb FROM 'P:\Project_LD_Server\Data\race_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
SELECT * FROM race_tb
GO

USE game_database
CREATE TABLE tier_tb
(
	id					INT NOT NULL IDENTITY(1,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

BULK INSERT tier_tb FROM 'P:\Project_LD_Server\Data\tier_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
SELECT * FROM tier_tb
GO