SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_game_character_sp(���� ĳ���� �ҷ�����)
--
-- Params
-- @character_id			: ���� ĳ���� ���̵�
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

		--ĳ���� ���� Ȯ��
		IF NOT EXISTS (SELECT 1 FROM character_tb WHERE id=@character_id)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 2003
			END

		--���� ĳ���� �ε�
		SELECT DISTINCT
		c.name,
		a.race_id, a.character_calss_id, a.skin_color, a.hair_color, a.eye_color, a.eyebrow_color,
		e.hair, e.helmet, e.shoulders, e.chest, e.bracers, e.hands, e.pants, e.boots, e.weapon_l, e.weapon_r,
		l.level, l.experience
		FROM character_tb			AS c
		INNER JOIN appearance_tb	AS a ON c.id = a.character_id
		INNER JOIN eqipment_tb		AS e ON c.id = e.character_id
		INNER JOIN level_tb			AS l ON c.id = l.character_id
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
	EXEC dbo.load_game_character_sp 0

	--UPDATE eqipment_tb SET hair=31		WHERE character_id=0
	--UPDATE eqipment_tb SET helmet=31		WHERE character_id=0
	--UPDATE eqipment_tb SET shoulders=51	WHERE character_id=0
	--UPDATE eqipment_tb SET chest=71		WHERE character_id=0
	--UPDATE eqipment_tb SET bracers=91		WHERE character_id=0
	--UPDATE eqipment_tb SET hands=111		WHERE character_id=0
	--UPDATE eqipment_tb SET pants=131		WHERE character_id=0
	--UPDATE eqipment_tb SET boots=151		WHERE character_id=0
	--UPDATE eqipment_tb SET weapon_l=0		WHERE character_id=0
	--UPDATE eqipment_tb SET weapon_r=0		WHERE character_id=0

END
GO