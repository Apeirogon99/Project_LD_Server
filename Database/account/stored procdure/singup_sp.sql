SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	singin(�α���)
--
-- Params
-- @id						: ���̵�
-- @password				: �н�����
-- @local					: �̸��� ����
-- @domain					: �̸��� ������
-- @global_id				: �۷ι� ���̵�
-- =============================================
USE account_database;
DROP PROCEDURE IF EXISTS singup_sp;
GO

CREATE PROCEDURE singup_sp
	@id				NVARCHAR(24),
	@password		NVARCHAR(24),
	@local			NVARCHAR(64),
	@domain			NVARCHAR(255),
	@global_id		INT OUTPUT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DECLARE @current_date AS SMALLDATETIME = CAST(GETUTCDATE() AS SMALLDATETIME);

		--���̵� �ߺ�
		IF EXISTS (SELECT 1 FROM account_table WHERE id=@id)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 1002
			END

		--�̸��� �ߺ�
		IF EXISTS (SELECT 1 FROM email_table WHERE local=@local AND domain=@domain)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 1003
			END

		--ȸ������
		INSERT INTO account_table (id, password) VALUES (@id, @password);
		
		SELECT @global_id=global_id FROM account_table WHERE id=@id AND password=@password
		INSERT INTO email_table(global_id, local, domain) VALUES (@global_id, @local, @domain);
		INSERT INTO information_table (global_id, create_date, update_date) VALUES (@global_id, @current_date, @current_date);

		EXEC account_database.dbo.send_email_verfiy_sp @global_id, @local, @domain
		
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

	EXEC dbo.singup_sp 'TEST_0002', '1234', 'gwanho0218', 'naver.com', @global_id OUTPUT

	SELECT * FROM account_table
	SELECT * FROM email_table
	SELECT * FROM information_table
END
GO