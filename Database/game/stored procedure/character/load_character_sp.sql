SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_character_sp(캐릭터 불러오기)
--
-- Params
-- @global_id			: 유저 글로벌 아이디
-- @server_id			: 유저 서버 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS load_character_sp;
GO

CREATE PROCEDURE load_character_sp
	@global_id		INT,
	@server_id		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		--캐릭터 정보 확인
		DECLARE @character_id AS INT;
		IF NOT EXISTS (SELECT 1 FROM character_tb WHERE global_id=@global_id AND server_id=@server_id)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 2003
			END

		--커스텀 정보 및 위치정보
		SELECT c.id, c.name, a.character_calss_id, a.race_id, a.seat, a.skin_color, a.hair_color, a.eye_color, a.eyebrow_color, e.hair, e.helmet, e.shoulders, e.pants, e.hands, e.chest, e.bracers, e.boots, e.weapon_l, e.weapon_r 
		FROM character_tb AS c INNER JOIN appearance_tb AS a ON (c.id = a.character_id) INNER JOIN eqipment_tb AS e ON (a.character_id = e.character_id)
		WHERE c.global_id=@global_id AND c.server_id=@server_id

		--SELECT e.ears, e.hair, e.facials_01, e.facials_02, e.helmet, e.shoulders, e.skirt, e.pants_add, e.legs_add, e.hands_add, e.chest_add, e.cape, e.bracers_add, e.boots, e.belt, e.tabard, e.weapon_l, e.weapon_r 
		--FROM character_tb AS c INNER JOIN eqipment_tb AS e ON (c.id = e.character_id) 
		--WHERE c.global_id=@global_id AND c.server_id=@server_id

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

--TEST
BEGIN
	USE game_database;
	--EXEC dbo.load_character_sp 1, 1
END
GO