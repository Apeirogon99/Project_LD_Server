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
-- @action				: 액션 (미정 = 0, 요청 = 1, 응답 = 2)
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
		
		--친구 아이디 검색
		DECLARE @friend_id INT
		SELECT @friend_id=id FROM character_tb WHERE name=@friend_name

		IF @friend_id IS NULL
			BEGIN
				ROLLBACK TRANSACTION;
				return 4001
			END

		--이미 친구인지 확인
		IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@friend_id AND friend_character_id=@character_id AND is_friend=1)
			BEGIN
				ROLLBACK TRANSACTION;
				return 4003
			END

		--action이 요청일 경우
		IF @action=1
			BEGIN
				--상대방이 이미 요청을 걸어두었을 경우
				IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_request=2)
					BEGIN
						--자신한테는 친구1 요청0으로 변경해야함
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@character_id AND friend_character_id=@friend_id

						--상대편은 정보가 없었으니까 Insert해야함
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
					END
				--이전에 자신이 이미 요청이 되어있는지 확인
				ELSE IF NOT EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_request=1)
					BEGIN
						--친구쪽에다가 응답을 자신한테는 요청을 Insert함
						INSERT friend_tb (user_character_id, friend_character_id, is_friend, is_request, is_block) VALUES (@friend_id, @character_id, 0, 2, 0)
						INSERT friend_tb (user_character_id, friend_character_id, is_friend, is_request, is_block) VALUES (@character_id, @friend_id, 0, 1, 0)
					END
				ELSE
					BEGIN
						ROLLBACK TRANSACTION;
						return 4005
					END
			END
		ELSE IF @action=2
			BEGIN
				--자신한테 요청이 들어와 있었는지 확인
				IF EXISTS (SELECT 1 FROM friend_tb WHERE user_character_id=@character_id AND friend_character_id=@friend_id AND is_request=2)
					BEGIN
						--자신한테는 친구1 요청0으로 변경해야함
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@character_id AND friend_character_id=@friend_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@character_id AND friend_character_id=@friend_id

						--상대편은 정보가 없었으니까 Insert해야함
						UPDATE friend_tb SET is_friend=1 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
						UPDATE friend_tb SET is_request=0 WHERE user_character_id=@friend_id AND friend_character_id=@character_id
					END
				ELSE
					BEGIN
						ROLLBACK TRANSACTION;
						return 4006
					END
			END
		ELSE
			BEGIN
				--유효하지 않은 액션
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

DECLARE @character_id	INT			= 2
DECLARE @friend_name	NVARCHAR(8) = '욘두'
DECLARE @ret			INT

EXEC @ret=request_friend_sp @character_id, @friend_name, 2
SELECT * FROM friend_tb
SELECT @ret AS '결과'

GO