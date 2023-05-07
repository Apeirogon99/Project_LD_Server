SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	singin(로그인)
--
-- Params
-- @id						: 아이디
-- @password				: 패스워드
-- @local					: 이메일 로컬
-- @domain					: 이메일 도메인
-- @global_id				: 글로벌 아이디
-- =============================================
USE account_database;
DROP PROCEDURE IF EXISTS singup_sp;
GO

CREATE PROCEDURE singup_sp
	@name			NVARCHAR(24),
	@password		NVARCHAR(24),
	@local			NVARCHAR(64),
	@domain			NVARCHAR(255),
	@global_id		INT OUTPUT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DECLARE @current_date AS DATETIME = SYSDATETIME()

		--아이디 중복
		IF EXISTS (SELECT 1 FROM user_tb WHERE name=@name)
			BEGIN
				RETURN 1002
			END

		--이메일 중복
		IF EXISTS (SELECT 1 FROM confirm_email_tb WHERE local=@local AND domain=@domain)
			BEGIN
				ROLLBACK TRANSACTION;
				RETURN 1003
			END

		--회원가입
		INSERT INTO user_tb (name, enable) VALUES (@name, 0);

		SELECT @global_id=global_id FROM user_tb WHERE name=@name
		INSERT INTO auth_password_tb (global_id, password, salt, update_date) VALUES (@global_id, @password, '0', @current_date)
		INSERT INTO confirm_email_tb(global_id, local, domain, verify_code, verify, expired_date) VALUES (@global_id, @local, @domain, NULL, 0, @current_date);
		INSERT INTO profile_tb(global_id, create_date, update_date) VALUES (@global_id, @current_date, @current_date);

		IF (@domain <> 'example.com')
			BEGIN
				EXEC account_database.dbo.send_email_verfiy_sp @global_id, @local, @domain
			END
		ELSE
			BEGIN
				UPDATE user_tb SET enable=1 WHERE global_id=@global_id
				RETURN 1007;
			END
		
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

	EXEC dbo.singup_sp 'TEST_0000', '1234', 'TEST_0000', 'example.com', @global_id OUTPUT

	print @global_id
	SELECT * FROM user_tb
	SELECT * FROM confirm_email_tb
	SELECT * FROM profile_tb
END
GO