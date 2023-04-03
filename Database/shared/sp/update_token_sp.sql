SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_token_sp(토큰 업데이트)
--
-- INPUT Params
-- @global_id		: 글로벌 아이디
-- @token			: 토큰
-- =============================================
USE shared_database;
DROP PROCEDURE IF EXISTS update_token_sp;
GO

CREATE PROCEDURE update_token_sp
	@global_id		INT,
	@token			NVARCHAR(36)
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		-- 기존에 정보가 남아있다면 업데이트, 없으면 삽입
		IF EXISTS (SELECT token FROM token_tb WHERE global_id=@global_id)
			BEGIN
				UPDATE token_tb SET token=@token WHERE global_id=@global_id
			END
		ELSE
			BEGIN
				INSERT INTO token_tb (global_id, token) VALUES (@global_id, @token)
			END

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

USE shared_database

DECLARE @new_token AS UNIQUEIDENTIFIER = NEWID();
DECLARE @token AS CHAR(36) = CONVERT(CHAR(36), @new_token)
EXEC update_token_sp 1, @token

SELECT * FROM token_tb

GO