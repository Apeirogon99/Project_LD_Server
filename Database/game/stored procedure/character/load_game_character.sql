SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_game_character_sp(게임 캐릭터 불러오기)
--
-- Params
-- @character_id			: 유저 캐릭터 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS load_game_character_sp;
GO

CREATE PROCEDURE load_game_character_sp
	@character_id	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		--캐릭터 정보 확인
		IF NOT EXISTS (SELECT 1 FROM character_tb WHERE id=@character_id)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 2003
			END

		--게임 캐릭터 로드
		SELECT DISTINCT
		c.name,
		a.race_id, a.character_calss_id, a.skin_color, a.hair_color, a.eye_color, a.eyebrow_color,
		e.hair, e.helmet, e.shoulders, e.chest, e.bracers, e.hands, e.pants, e.boots, e.weapon_l, e.weapon_r,
		l.level, l.experience,
		m.amount
		FROM character_tb			AS c
		INNER JOIN appearance_tb	AS a ON c.id = a.character_id
		INNER JOIN eqipment_tb		AS e ON c.id = e.character_id
		INNER JOIN level_tb			AS l ON c.id = l.character_id
		INNER JOIN money_tb			AS m ON c.id = m.character_id
		WHERE c.id=@character_id

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
	EXEC dbo.load_game_character_sp 1

END
GO