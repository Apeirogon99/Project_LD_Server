USE game_database

DROP TABLE IF EXISTS appearance_tb;
DROP TABLE IF EXISTS eqipment_tb;
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
	race_id				TINYINT NULL FOREIGN KEY REFERENCES race_tb(id)				ON DELETE SET NULL,
	character_calss_id	TINYINT NULL FOREIGN KEY REFERENCES character_class_tb(id)	ON DELETE SET NULL,
	seat				TINYINT NOT NULL,
	skin_color			INT NOT NULL DEFAULT 0,
	hair_color			INT NOT NULL DEFAULT 0,
	eye_color			INT NOT NULL DEFAULT 0,
	eyebrow_color		INT NOT NULL DEFAULT 0,
)
GO

CREATE TABLE eqipment_tb
(
	character_id		INT NOT NULL FOREIGN KEY REFERENCES character_tb(id) ON DELETE CASCADE,

--	ears				INT NOT NULL DEFAULT 0,
	hair				INT NOT NULL DEFAULT 0,
--	facials_01			INT NOT NULL DEFAULT 0,
--	facials_02			INT NOT NULL DEFAULT 0,
	helmet				INT NOT NULL DEFAULT 0,
	shoulders			INT NOT NULL DEFAULT 0,
--	skirt				INT NOT NULL DEFAULT 0,
	chest_add			INT NOT NULL DEFAULT 0,
	bracers_add			INT NOT NULL DEFAULT 0,
	hands_add			INT NOT NULL DEFAULT 0,
	pants_add			INT NOT NULL DEFAULT 0,
--	legs_add			INT NOT NULL DEFAULT 0,
--	cape				INT NOT NULL DEFAULT 0,
	boots				INT NOT NULL DEFAULT 0,
--	belt				INT NOT NULL DEFAULT 0,
--	tabard				INT NOT NULL DEFAULT 0,
	weapon_l			INT NOT NULL DEFAULT 0,
	weapon_r			INT NOT NULL DEFAULT 0,
);
GO

SELECT * FROM character_tb
SELECT * FROM appearance_tb
SELECT * FROM eqipment_tb
GO