SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	delete_character_sp(ĳ���� �г��� ����)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS delete_character_sp;
GO

CREATE PROCEDURE delete_character_sp
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

		DELETE FROM character_tb WHERE id=@character_id

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO