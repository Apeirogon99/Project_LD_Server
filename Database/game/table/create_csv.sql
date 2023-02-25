USE game_database

DROP TABLE IF EXISTS item_table;
DROP TABLE IF EXISTS class_table;
DROP TABLE IF EXISTS category_table;
GO

USE game_database
CREATE TABLE category_table
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	name				NVARCHAR(32) NOT NULL,
)

--BULK INSERT category_table FROM '.\..\csv\category_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
GO

USE game_database
CREATE TABLE item_table
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	category_id			INT NOT NULL FOREIGN KEY REFERENCES category_table (id),
	name				NVARCHAR(32) NULL DEFAULT '',
	cost				INT NULL DEFAULT 0,
	minimum_level		INT NULL DEFAULT 0,
	max					INT NULL DEFAULT 1,
	size_x				INT NULL DEFAULT 0,
	size_y				INT NULL DEFAULT 0
)

BULK INSERT item_table FROM 'P:\ApeirogonEngine\ApeirogonEngine\Database\game\csv\item_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
SELECT * FROM item_table
GO

USE game_database
CREATE TABLE class_table
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	category_id			INT NOT NULL FOREIGN KEY REFERENCES category_table (id),
)

--BULK INSERT class_table FROM '.\..\csv\class_datas.csv' WITH (FIRSTROW=2, KEEPNULLS, FORMAT='CSV')
GO