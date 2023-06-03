SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	check_email_verify(이메일 체크)
--
-- Params
-- @global_id				: 글로벌 아이디
-- @code					: 이메일 인증 번호
-- =============================================
USE account_database;
DROP PROCEDURE IF EXISTS check_email_verify_sp;
GO

CREATE PROCEDURE check_email_verify_sp
	@global_id		INT,
	@code			INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DECLARE @temp_verify	AS INT
		DECLARE @temp_code		AS INT
		DECLARE @temp_local		AS NVARCHAR(64)
		DECLARE @temp_domain	AS NVARCHAR(255)
		DECLARE @temp_date		AS DATETIME	

		SELECT @temp_verify=verify, @temp_code=verify_code, @temp_local=local, @temp_domain=domain, @temp_date=expired_date
		FROM confirm_email_tb
		WHERE global_id=@global_id

		--이메일 인증 중복 확인
		IF (@temp_verify = 1)
			BEGIN
				RETURN 1005
			END

		--이메일 임시 키 확인
		IF (@temp_code IS NULL)
			BEGIN
				EXEC dbo.send_email_verfiy_sp @global_id, @temp_local, @temp_domain;
				RETURN 1007
			END

		--이메일 인증 유효기간 확인
		DECLARE @expired AS INT = 10
		IF (@temp_date <= DATEADD(DAY, -10, SYSDATETIME()))
			BEGIN
				EXEC dbo.send_email_verfiy_sp @global_id, @temp_local, @temp_domain;
				RETURN 1007
			END

		--이메일 인증 확인
		IF (@temp_code <> @code)
			BEGIN
				RETURN 1006
			END

		--인증 번호 맞음
		UPDATE confirm_email_tb SET verify=1 WHERE global_id=@global_id
		UPDATE confirm_email_tb SET verify_code=NULL WHERE global_id=@global_id 
		UPDATE user_tb SET enable = 1
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

	--UPDATE email_table SET verify=0 WHERE global_id=2
	--DECLARE @ret INT
	--EXEC @ret=dbo.check_email_verify_sp 1, '623972'
	--print @ret
END
GO