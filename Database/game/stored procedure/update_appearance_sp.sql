SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_appearance_sp(캐릭터 외형 변경)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @skin_color			: 스킨 색깔
-- @hair_color			: 헤어 색깔
-- @eye_color			: 눈 색깔
-- @eyebrow_color		: 눈썹 색깔
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS update_appearance_sp;
GO

CREATE PROCEDURE update_appearance_sp
	@character_id	INT,
	@skin_color		INT,
	@hair_color		INT,
	@eye_color		INT,
	@eyebrow_color	INT
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

		UPDATE appearance_tb SET skin_color=@skin_color, hair_color=@hair_color, eye_color=@eye_color, eyebrow_color=@eyebrow_color WHERE character_id=@character_id

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO