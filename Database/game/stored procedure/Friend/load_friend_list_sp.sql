SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_friend_list_sp(친구 로드)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @list_type			: 리스트 타입
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS load_friend_list_sp;
GO

CREATE PROCEDURE load_friend_list_sp
	@character_id	INT,
	@list_type		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DECLARE @is_friend	INT
		DECLARE @is_request INT
		DECLARE @is_block	INT

		IF @list_type=0
			BEGIN
				SET @is_friend	= 1
				SET @is_request = 0
				SET @is_block	= 0
			END
		ELSE IF @list_type=1
			BEGIN
				SET @is_friend	= 0
				SET @is_request = 2
				SET @is_block	= 0
			END
		ELSE IF @list_type=2
			BEGIN
				SET @is_friend	= 1
				SET @is_request = 0
				SET @is_block	= 1
			END
		ELSE
			BEGIN
				ROLLBACK TRANSACTION;
				return 4009
			END

		SELECT DISTINCT
		f.friend_character_id,
		c.name,
		a.character_calss_id,
		l.level,
		t.locale
		FROM friend_tb				AS f
		INNER JOIN character_tb		AS c ON f.friend_character_id = c.id
		INNER JOIN appearance_tb	AS a ON f.friend_character_id = a.character_id
		INNER JOIN level_tb			AS l ON f.friend_character_id = l.character_id
		INNER JOIN trace_tb			AS t ON f.friend_character_id = t.character_id
		WHERE user_character_id=@character_id AND is_friend=@is_friend AND is_request=@is_request AND is_block=@is_block
		ORDER BY c.name ASC
		
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

DECLARE @character_id	INT = 2
DECLARE @list_type		INT = 2
DECLARE @ret			INT

EXEC @ret=load_friend_list_sp @character_id, @list_type
print @ret


--INSERT INTO friend_tb (user_character_id, friend_character_id, is_friend, action) VALUES (1, 2, 1, 0) -- 친구 상태
--INSERT INTO friend_tb (user_character_id, friend_character_id, is_friend, action) VALUES (1, 3, 0, 1) -- 요청 상태
--INSERT INTO friend_tb (user_character_id, friend_character_id, is_friend, action) VALUES (1, 4, 1, 1) -- 차단 상태
--SELECT * FROM friend_tb

GO