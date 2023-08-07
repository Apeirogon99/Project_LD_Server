SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	find_friend_sp(친구 찾기)
--
-- Params
-- @friend_name			: 친구 이름
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS find_friend_sp;
GO

CREATE PROCEDURE find_friend_sp
	@friend_name	NVARCHAR(8),
	@friend_id		INT OUTPUT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		SELECT @friend_id=id FROM character_tb WHERE name=@friend_name

		IF @friend_id IS NULL
			BEGIN
				return 4001
			END

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

-- ==================================== --
--										--
--				  TEST					--
--										--
-- ==================================== --
USE game_database;

DECLARE @friend_name	NVARCHAR(8) = '몰라'
DECLARE @friend_id		INT
DECLARE @ret			INT

EXEC @ret=find_friend_sp @friend_name, @friend_id OUTPUT
SELECT @ret AS '결과'
SELECT @friend_id AS '아이디'

GO