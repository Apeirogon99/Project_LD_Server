SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	select_character_sp(캐릭터 조회)
--
-- Params
-- @global_id		: 글로벌 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS select_character_sp;
GO

CREATE PROCEDURE select_character_sp
	@global_id	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		SELECT server_id, COUNT(*) AS character_count
		FROM character_tb 
		WHERE global_id=@global_id
		GROUP BY server_id

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

EXEC select_character_sp 1
GO