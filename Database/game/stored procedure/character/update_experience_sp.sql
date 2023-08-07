SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_experience_sp(레벨 업데이트)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @level				: 레벨
-- @experience			: 경험치
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS update_experience_sp;
GO

CREATE PROCEDURE update_experience_sp
	@character_id	INT,
	@level			INT,
	@experience		INT
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

		UPDATE level_tb SET level=@level WHERE character_id=@character_id
		UPDATE level_tb SET experience=@experience WHERE character_id=@character_id

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO