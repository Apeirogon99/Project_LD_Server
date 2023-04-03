SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_nickname_sp(ĳ���� �г��� ����)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @new_name			: ���ο� �̸�
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS update_name_sp;
GO

CREATE PROCEDURE update_name_sp
	@character_id	INT,
	@new_name		NVARCHAR(8)
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

		UPDATE character_tb SET name=@new_name WHERE id=@character_id

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO