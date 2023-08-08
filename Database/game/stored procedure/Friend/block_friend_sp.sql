SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	block_friend_sp(친구 차단)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @friend_name			: 친구 이름
-- @action				: 액션 (미정 = 0, 차단 = 1, 해제 = 2)
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS block_friend_sp;
GO

CREATE PROCEDURE block_friend_sp
	@character_id	INT,
	@friend_name	NVARCHAR(8),
	@action			INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		--친구 아이디 검색
		DECLARE @friend_id INT
		SELECT @friend_id=id FROM character_tb WHERE name=@friend_name

		IF @friend_id IS NULL
			BEGIN
				ROLLBACK TRANSACTION;
				return 4001
			END

		--친구가 맞는지 검색
		IF NOT EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_friend=1)
			BEGIN
				ROLLBACK TRANSACTION;
				return 4002
			END

		IF @action=1
			BEGIN
				UPDATE friend_tb SET is_block=1 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
			END
		ELSE IF @action=2
			BEGIN
				UPDATE friend_tb SET is_block=0 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
			END
		ELSE
			BEGIN
				ROLLBACK TRANSACTION;
				return 4009
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

DECLARE @character_id	INT = 1
DECLARE @ret			INT

EXEC @ret=block_friend_sp 1, '민트초코', 0
print @ret

SELECT * FROM friend_tb WHERE user_character_id=1

GO