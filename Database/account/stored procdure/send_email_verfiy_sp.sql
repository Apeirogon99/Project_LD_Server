SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	send_email_verify(�̸��� üũ)
--
-- Params
-- @global_id				: �۷ι� ���̵�
-- @local					: �̸��� ����
-- @domain					: �̸��� ������
-- =============================================
USE account_database
DROP PROCEDURE IF EXISTS send_email_verfiy_sp;
GO

CREATE PROCEDURE send_email_verfiy_sp
	@global_id		INT,
	@local			NVARCHAR(64),
	@domain			NVARCHAR(255)
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;
		
		--�̸��� ��ġ��
		DECLARE @email AS NVARCHAR(319)
		SET @email = @local + '@' + @domain

		--���� ���� ����
		DECLARE @temp_verify AS INT = CONVERT(INT, RAND() * 1000000)
		UPDATE confirm_email_tb SET verify_code=@temp_verify WHERE global_id=@global_id

		--�̸��� ������
		DECLARE @email_body AS NVARCHAR(255) = '�̸��� ���� ��ȣ [ ' + CONVERT(NVARCHAR(7), @temp_verify) + ' ]'
		EXEC msdb.dbo.sp_send_dbmail @profile_name='database_smtp', @recipients=@email, @subject='Project_LD �̸��� ����', @body=@email_body
		UPDATE confirm_email_tb SET expired_date=SYSDATETIME()

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

	EXEC dbo.send_email_verfiy_sp 2, 'gksdidxornjs', 'naver.com'

	SELECT * FROM user_tb
	SELECT * FROM confirm_email_tb
	SELECT * FROM profile_tb

	
END
GO