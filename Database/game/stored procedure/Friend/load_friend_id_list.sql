SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_friend_id_list_sp(친구 아이디만 불러오기)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS load_friend_id_list_sp;
GO

CREATE PROCEDURE load_friend_id_list_sp
	@character_id	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		SELECT
		f.friend_character_id
		FROM friend_tb AS f
		WHERE is_friend=1 AND user_character_id=@character_id
		ORDER BY f.id ASC

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

EXEC @ret=load_friend_id_list_sp @character_id
print @ret

GO