SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_skill_tree_sp(스킬 트리 불러오기)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS load_skill_tree_sp;
GO

CREATE PROCEDURE load_skill_tree_sp
	@character_id	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		SELECT
		s.skill_id, s.count
		FROM skill_tb AS s
		WHERE character_id=@character_id

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
DECLARE @ret			INT

EXEC @ret=load_skill_tree_sp @character_id
print @ret

GO