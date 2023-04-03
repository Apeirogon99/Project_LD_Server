SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_appearance_sp(ĳ���� ���� ����)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @skin_color			: ��Ų ����
-- @hair_color			: ��� ����
-- @eye_color			: �� ����
-- @eyebrow_color		: ���� ����
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

		--ĳ���� ���� Ȯ��
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