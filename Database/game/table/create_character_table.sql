USE game_database

DROP TABLE IF EXISTS appearance_tb;
DROP TABLE IF EXISTS eqipment_tb;
DROP TABLE IF EXISTS inventory_tb;
DROP TABLE IF EXISTS money_tb;
DROP TABLE IF EXISTS friend_tb;
DROP TABLE IF EXISTS level_tb;
DROP TABLE IF EXISTS trace_tb;
DROP TABLE IF EXISTS character_tb;
GO

CREATE TABLE character_tb
(
	id					INT NOT NULL IDENTITY(0,1) PRIMARY KEY,
	name				NVARCHAR(8) NOT NULL,
	global_id			INT NOT NULL,
	server_id			INT NOT NULL,
)
GO

CREATE TABLE appearance_tb
(
	character_id		INT NOT NULL FOREIGN KEY REFERENCES character_tb(id)		ON DELETE CASCADE,

	race_id				INT NOT NULL ,
	character_calss_id	INT NOT NULL ,
	seat				INT NOT NULL DEFAULT 0,
	skin_color			INT NOT NULL DEFAULT 0,
	hair_color			INT NOT NULL DEFAULT 0,
	eye_color			INT NOT NULL DEFAULT 0,
	eyebrow_color		INT NOT NULL DEFAULT 0,
)
GO

CREATE TABLE eqipment_tb
(
	character_id		INT NOT NULL FOREIGN KEY REFERENCES character_tb(id) ON DELETE CASCADE,

	hair				INT NOT NULL DEFAULT 0,
	helmet				INT NOT NULL DEFAULT 0,
	shoulders			INT NOT NULL DEFAULT 0,
	chest				INT NOT NULL DEFAULT 0,
	bracers				INT NOT NULL DEFAULT 0,
	hands				INT NOT NULL DEFAULT 0,
	pants				INT NOT NULL DEFAULT 0,
	boots				INT NOT NULL DEFAULT 0,
	weapon_l			INT NOT NULL DEFAULT 0,
	weapon_r			INT NOT NULL DEFAULT 0,
);
GO


CREATE TABLE inventory_tb
(
	id					INT		NOT NULL IDENTITY(0,1) PRIMARY KEY,
	character_id		INT		NOT NULL FOREIGN KEY REFERENCES character_tb(id)	ON DELETE CASCADE,
	item_code			INT		NOT NULL ,
	inven_pos_x			INT		NOT NULL ,
	inven_pos_y			INT		NOT NULL ,
	rotation			INT		NOT NULL CHECK (rotation = 1 OR rotation = 0),
);
Go

CREATE TABLE money_tb
(
	id					INT		NOT NULL IDENTITY(0,1) PRIMARY KEY,
	character_id		INT		NOT NULL FOREIGN KEY REFERENCES character_tb(id)	ON DELETE CASCADE,
	amount				INT		NOT NULL DEFAULT 0,
);
GO

CREATE TABLE friend_tb
(
	id					INT		NOT NULL	IDENTITY(0,1) PRIMARY KEY,
	user_character_id	INT		NOT NULL	FOREIGN KEY REFERENCES character_tb(id)	ON DELETE CASCADE,
	friend_character_id	INT		NOT NULL	DEFAULT 0,
	is_friend			INT		NOT NULL	CHECK (is_friend = 0 OR is_friend = 1),
	action				INT		NOT NULL	CHECK (action = 0 OR action = 1), -- 0 = 없음, 1 = 요청, 2 = 차단
);
GO

CREATE TABLE level_tb
(
	character_id		INT	NOT NULL FOREIGN KEY REFERENCES character_tb(id)	ON DELETE CASCADE,
	level				INT NOT NULL DEFAULT 1,
	experience			INT NOT NULL DEFAULT 0
)
GO

CREATE TABLE trace_tb
(
	character_id		INT		NOT NULL FOREIGN KEY REFERENCES character_tb(id)	ON DELETE CASCADE,
	locale				INT		NOT NULL,
	world_pos_x			FLOAT	NOT NULL,
	world_pos_y			FLOAT	NOT NULL,
	world_pos_z			FLOAT	NOT NULL,
)
GO

SELECT * FROM character_tb
SELECT * FROM appearance_tb
SELECT * FROM eqipment_tb
SELECT * FROM inventory_tb
SELECT * FROM money_tb
SELECT * FROM level_tb
GO