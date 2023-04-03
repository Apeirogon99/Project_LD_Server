USE game_database

DROP TABLE IF EXISTS item;
DROP TABLE IF EXISTS character_class;
DROP TABLE IF EXISTS race;
DROP TABLE IF EXISTS category;
GO

USE game_database
CREATE TABLE category
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

--BULK INSERT category FROM '.\..\csv\category_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
GO

USE game_database
CREATE TABLE item
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	category_id			INT NOT NULL FOREIGN KEY REFERENCES category (id),
	name				NVARCHAR(32) NULL DEFAULT '',
	cost				INT NULL DEFAULT 0,
	minimum_level		INT NULL DEFAULT 0,
	max					INT NULL DEFAULT 1,
	size_x				INT NULL DEFAULT 0,
	size_y				INT NULL DEFAULT 0
)

--BULK INSERT item FROM 'P:\ApeirogonEngine\ApeirogonEngine\Database\game\csv\item_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
GO

USE game_database
CREATE TABLE character_class
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

--BULK INSERT character_class FROM '.\..\csv\class_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
GO

USE game_database
CREATE TABLE race_tb
(
	id					TINYINT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

--BULK INSERT character_class FROM '.\..\csv\class_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
GO