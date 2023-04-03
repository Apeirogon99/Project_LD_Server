SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	auth_password_sp(비밀번호 인증)
--
-- INPUT Params
-- @name				: 아이디
-- @password			: 패스워드
-- @global_id			: 글로벌 아이디
-- =============================================
USE account_database;
DROP PROCEDURE IF EXISTS auth_password_sp;
GO

CREATE PROCEDURE auth_password_sp
	@name			VARCHAR(24),
	@password		VARCHAR(24),
	@global_id		INT,
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		--커맨드 조회
		DECLARE @temp_name		AS NVARCHAR(24)
		DECLARE @temp_enable	AS INT
		SELECT @global_id=global_id, @temp_name=name, @temp_enable=enable FROM user_tb WHERE name=@name

		--아이디 존재 유무
		IF (@temp_name=@name)
			BEGIN
				RETURN 1001
			END

		--이메일 인증 체크
		IF (@temp_enable = 0)
			BEGIN
				DECLARE @temp_local		AS NVARCHAR(64)
				DECLARE @temp_domain	AS NVARCHAR(255)

				SELECT @temp_local=local, @temp_domain=domain FROM confirm_email_tb WHERE global_id=@global_id

				EXEC dbo.send_email_verfiy_sp @global_id, @temp_local, @temp_domain;

				RETURN 1004
			END

		--비밀번호 조회


		--티켓 전달
		DECLARE @new_token AS UNIQUEIDENTIFIER = NEWID();
		UPDATE shared_database.dbo.token_tb SET token=@new_token WHERE id=@global_id;
		SET @token = CONVERT(NVARCHAR(36), @new_token)

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO