SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	delete_token_sp(로그인)
--
-- INPUT Params
-- @global_id		: 글로벌 아이디
-- =============================================
USE shared_database;
DROP PROCEDURE IF EXISTS delete_token_sp;
GO

CREATE PROCEDURE delete_token_sp
	@global_id		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		-- 기존에 정보가 남아있다면 삭제
		IF EXISTS (SELECT token FROM token_tb WHERE global_id=@global_id)
			BEGIN
				DELETE FROM token_tb WHERE global_id=@global_id;
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
USE shared_database
SELECT * FROM token_tb

EXEC delete_token_sp 1

SELECT * FROM token_tb
GO