SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	singin(�α���)
--
-- Params
-- @id					: ���̵�
-- @password			: �н�����
-- @global_id			: �۷ι� ���̵�
-- @ticket				: ��ū
-- =============================================
USE account_database;
DROP PROCEDURE IF EXISTS singin_sp;
GO

CREATE PROCEDURE singin_sp
	@id				VARCHAR(24),
	@password		VARCHAR(24),
	@global_id		INT OUTPUT,
	@ticket			NVARCHAR(36) OUTPUT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DECLARE @new_ticket AS UNIQUEIDENTIFIER = NEWID();
		--DECLARE @global_id AS INT

		--���̵� ���� ����
		IF NOT EXISTS (SELECT 1 FROM account_table WHERE id=@id AND password=@password)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 1001
			END

		--global_id Ȯ��
		SELECT @global_id=global_id FROM account_table WHERE id=@id AND password=@password 

		--�̸��� ���� üũ (�ȵ������� �۷ι� ���̵� �༭ �������ְ���)
		IF (SELECT verify FROM account_table AS account INNER JOIN email_table AS email ON (account.global_id = email.global_id) WHERE account.id=@id AND account.password=@password) = 0
			BEGIN
				RETURN 1004
			END


		--Ƽ�� ����
		UPDATE account_table SET ticket=@new_ticket WHERE id=@id AND password=@password;
		SET @ticket = CONVERT(NVARCHAR(36), @new_ticket)

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
	USE account_database;

	DECLARE @global_id AS INT
	DECLARE @ticket AS NVARCHAR(36)

	UPDATE email_table SET verify=1 WHERE global_id=2

	EXEC dbo.singin_sp 'TEST_0001', '1234', @global_id OUTPUT, @ticket OUTPUT

	print @global_id
	print @ticket;

	SELECT * FROM account_table
	SELECT * FROM email_table
	SELECT * FROM information_table
END
GO