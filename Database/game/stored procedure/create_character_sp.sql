SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	create_character_sp(새로운 캐릭터 생성)
--
-- Params
-- @name				: 유저 닉네임
-- @class				: 유저 클래스
-- @race				: 유저 종족
-- @seat				: 캐릭터 선택창 위치
-- @global_id			: 유저 글로벌 아이디
-- @server_id			: 유저 서버 아이디
--
-- @skin				: 피부색
-- @hair				: 머리색
-- @eye					: 눈색
-- @eyebrow				: 눈섭색
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS create_character_sp;
GO

CREATE PROCEDURE create_character_sp
	@name			NVARCHAR(8),
	@class			INT,
	@race			INT,
	@seat			INT,
	@global_id		INT,
	@server_id		INT,

	@skin_color		INT,
	@hair_color		INT,
	@eye_color		INT,
	@eyebrow_color	INT,

	@hair			INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		-- 초과된 캐릭터 수를 소유중
		DECLARE @current_character_count AS INT;
		DECLARE @charcater_max_count AS INT = 4;

		-- 해당 서버에 캐릭터 몇명인지 확인
		SELECT @current_character_count=COUNT(*) FROM character_tb WHERE global_id=@global_id AND server_id=@server_id;
		IF (@current_character_count > @charcater_max_count)
			BEGIN
				ROLLBACK TRANSACTION
				RETURN 2001
			END

		-- 위치 중복
		IF EXISTS (SELECT 1 FROM character_tb AS c INNER JOIN appearance_tb AS a ON c.id = a.character_id WHERE c.global_id=@global_id AND c.server_id=@server_id AND a.seat=@seat)
			BEGIN
				ROLLBACK TRANSACTION
				RETURN 2004
			END

		-- 닉네임 중복
		IF EXISTS (SELECT 1 FROM character_tb WHERE name=@name)
			BEGIN
				ROLLBACK TRANSACTION
				RETURN 2003
			END
		
		--캐릭터 생성
		INSERT INTO character_tb (global_id, server_id, name) VALUES (@global_id, @server_id, @name);

		--캐릭터 아이디 불러오기
		DECLARE @temp_character_id AS INT
		SELECT @temp_character_id=id FROM character_tb WHERE global_id=@global_id AND name=@name;

		INSERT INTO appearance_tb (character_id, race_id, character_calss_id, seat, skin_color, hair_color, eye_color, eyebrow_color)
		VALUES (@temp_character_id, @race, @class, @seat, @skin_color, @hair_color, @eye_color, @eyebrow_color)

		INSERT INTO eqipment_tb (character_id, hair) VALUES (@temp_character_id, @hair)

		COMMIT TRANSACTION
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION
	RETURN -1
END CATCH
GO

-- ==================================== --
--										--
--				  TEST					--
--										--
-- ==================================== --
USE game_database;

DECLARE @ret			INT

EXEC @ret=create_character_sp 'SP테스트', 1, 1, 0, 1, 1, 0, 0, 0, 0, 0
print @ret

SELECT * FROM character_tb

GO