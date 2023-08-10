SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_skill_tree_sp(스킬 트리 불러오기)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @skill_id			: 스킬 아이디
-- @skill_count			: 스킬 증가 카운트
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS update_skill_tree_sp;
GO

CREATE PROCEDURE update_skill_tree_sp
	@character_id	INT,
	@skill_id		INT,
	@skill_count	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		IF EXISTS (SELECT 1 FROM skill_tb WHERE character_id=@character_id AND skill_id=@skill_id)
			BEGIN

				DECLARE @cur_skill_count INT
				SELECT @cur_skill_count=count FROM skill_tb WHERE character_id=@character_id AND skill_id=@skill_id
				
				print @cur_skill_count

				SET @cur_skill_count += @skill_count

				print @cur_skill_count

				IF @cur_skill_count > 0
					BEGIN
						UPDATE skill_tb SET count=@cur_skill_count WHERE character_id=@character_id AND skill_id=@skill_id
					END
				ELSE
					BEGIN
						DELETE FROM skill_tb WHERE character_id=@character_id AND skill_id=@skill_id 
					END				
			END
		ELSE
			BEGIN
				INSERT INTO skill_tb (character_id, skill_id, count) VALUES (@character_id, @skill_id, 1)
			END

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

-- ==================================== --
--										--
--				  TEST					--
--										--
-- ==================================== --
USE game_database;

DECLARE @character_id	INT = 1
DECLARE @skill_id		INT = 1
DECLARE @skill_count	INT = -1
DECLARE @ret			INT

EXEC @ret=update_skill_tree_sp @character_id, @skill_id, @skill_count
print @ret

SELECT * FROM skill_tb WHERE character_id=@character_id

GO