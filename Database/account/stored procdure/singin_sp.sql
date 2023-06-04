SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	singin(�α���)
--
-- INPUT Params
-- @name				: ���̵�
-- @password			: �н�����

-- OUTPUT Params
-- @global_id			: �۷ι� ���̵�
-- @ticket				: ��ū
-- =============================================
USE account_database;
DROP PROCEDURE IF EXISTS singin_sp;
GO

CREATE PROCEDURE singin_sp
	@name			VARCHAR(24),
	@password		VARCHAR(24),
	@global_id		INT OUTPUT,
	@token			CHAR(36) OUTPUT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		--���̵� ���� ����
		IF NOT EXISTS (SELECT 1 FROM user_tb WHERE name=@name)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 1001
			END

		--Ŀ�ǵ� ��ȸ
		DECLARE @temp_password	AS INT
		DECLARE @temp_enable	AS INT

		SELECT @global_id=u.global_id, @temp_enable=u.enable, @temp_password=p.password
		FROM user_tb AS u INNER JOIN auth_password_tb AS p ON u.global_id=p.global_id
		WHERE u.name=@name

		--�̸��� ���� üũ
		IF (@temp_enable = 0)
			BEGIN
				DECLARE @temp_local		AS NVARCHAR(64)
				DECLARE @temp_domain	AS NVARCHAR(255)

				SELECT @temp_local=local, @temp_domain=domain FROM confirm_email_tb WHERE global_id=@global_id

				EXEC dbo.send_email_verfiy_sp @global_id, @temp_local, @temp_domain;
				ROLLBACK TRANSACTION;
				RETURN 1004
			END

		--��й�ȣ üũ
		IF (@temp_password<>@password)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 1001
			END

		--Ƽ�� CHAR(36)���� ��ȯ�� ����
		DECLARE @new_token AS UNIQUEIDENTIFIER = NEWID();
		SET @token = CONVERT(CHAR(36), @new_token)
		EXEC shared_database.dbo.update_token_sp @global_id, @token;

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

--TEST
USE account_database;


DECLARE @global_id AS INT
DECLARE @token AS CHAR(36)
DECLARE @result AS INT
EXEC @result = dbo.singin_sp 'TEST_ID', '1234', @global_id OUTPUT, @token OUTPUT

print 'global_id = ' + CAST(@global_id as VARCHAR(20))
print 'token     = ' + @token;
print 'result    = ' + CAST(@result as VARCHAR(20))

SELECT * FROM user_tb
SELECT * FROM auth_password_tb
SELECT * FROM confirm_email_tb
SELECT * FROM profile_tb

GO