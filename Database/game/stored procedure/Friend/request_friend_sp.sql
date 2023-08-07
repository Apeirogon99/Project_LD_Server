SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	request_friend_sp(친구 요청)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @friend_name			: 친구 이름
-- @action				: 액션
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS request_friend_sp;
GO

CREATE PROCEDURE request_friend_sp
	@character_id	INT,
	@friend_name	NVARCHAR(8),
	@action			INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DECLARE @friend_id INT
		SELECT @friend_id=id FROM character_tb WHERE name=@friend_name

		IF @friend_id IS NULL
			BEGIN
				ROLLBACK TRANSACTION;
				return 4001
			END

		DECLARE @cur_is_friend INT
		DECLARE @cur_action INT
		IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@friend_id AND friend_character_id=@character_id)
			BEGIN

				SELECT @cur_is_friend=is_friend, @cur_action=action FROM friend_tb WHERE user_character_id=@friend_id AND friend_character_id=@character_id

				IF @cur_is_friend=1 AND @cur_action=0
					BEGIN
						ROLLBACK TRANSACTION;
						return 4003
					END
				ELSE IF @cur_is_friend=1 AND @cur_action=1
					BEGIN
						ROLLBACK TRANSACTION;
						return 4004
					END
				ELSE IF @cur_is_friend=0 AND @cur_action=1 AND @action=1
					BEGIN
						ROLLBACK TRANSACTION;
						return 4005
					END
				ELSE IF @cur_is_friend=0 AND @cur_action=1 AND @action=0
					BEGIN
						DELETE FROM friend_tb WHERE user_character_id=@friend_id AND friend_character_id=@character_id
					END
			END
		ELSE
			BEGIN
				INSERT friend_tb (user_character_id, friend_character_id, is_friend, action) VALUES (@friend_id, @character_id, 0, 1)

				IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_friend=0 AND action=1) AND EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@friend_id AND friend_character_id=@character_id AND is_friend=0 AND action=1)
					BEGIN
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
					END
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

DECLARE @character_id	INT			= 1
DECLARE @friend_name	NVARCHAR(8) = '몰라유'
DECLARE @ret			INT

EXEC @ret=request_friend_sp @character_id, @friend_name, 1
SELECT * FROM friend_tb WHERE user_character_id=1
SELECT @ret AS '결과'

GO