SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	insert_inventory_sp(인벤토리 로드)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @item_code			: 아이템 아이디
-- @inven_pos_x			: 인벤 위치 x값
-- @inven_pos_y			: 인벤 위치 y값
-- @rotation			: 인벤 회전 r값
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS insert_inventory_sp;
GO

CREATE PROCEDURE insert_inventory_sp
	@character_id	INT,
	@item_code		INT,
	@inven_pos_x	INT,
	@inven_pos_y	INT,
	@rotation		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		INSERT INTO inventory_tb (character_id, item_code,inven_pos_x, inven_pos_y, rotation) 
		VALUES (@character_id, @item_code, @inven_pos_x, @inven_pos_y, @rotation)
		
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

--반복문
/*
DECLARE @character_id	INT = 0
DECLARE @rotation		INT = 1
DECLARE @ret			INT

DECLARE @NUM INT, @SUM INT,@TIME INT --변수선언
SET @NUM = 1
SET @SUM = 0
SET @TIME = 100

WHILE @NUM<@TIME
BEGIN 
EXEC @ret=insert_inventory_sp @character_id, @NUM, @NUM, @NUM, @NUM, @NUM, @NUM, @rotation
SET @NUM = @NUM+1
END;
*/

DECLARE @ret INT
EXEC @ret=insert_inventory_sp 0, 31, 0, 0, 0
EXEC @ret=insert_inventory_sp 0, 51, 2, 0, 0
EXEC @ret=insert_inventory_sp 0, 71, 0, 2, 1

SELECT * FROM inventory_tb

GO