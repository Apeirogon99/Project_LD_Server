SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	send_email_verify(이메일 체크)
--
-- Params
-- @global_id				: 글로벌 아이디
-- @local					: 이메일 로컬
-- @domain					: 이메일 도메인
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
		
		--이메일 합치기
		DECLARE @email AS NVARCHAR(319)
		SET @email = @local + '@' + @domain

		--랜덤 숫자 생성
		DECLARE @temp_verify AS INT = CONVERT(INT, RAND() * 1000000)
		UPDATE confirm_email_tb SET verify_code=@temp_verify WHERE global_id=@global_id

		--이메일 보내기
		DECLARE @email_body AS NVARCHAR(255) = '이메일 인증 번호 [ ' + CONVERT(NVARCHAR(7), @temp_verify) + ' ]'
		EXEC msdb.dbo.sp_send_dbmail @profile_name='database_smtp', @recipients=@email, @subject='Project_LD 이메일 인증', @body=@email_body
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