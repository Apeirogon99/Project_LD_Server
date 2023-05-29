SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_inventory_sp(인벤토리 변경)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @id					: 아이템 고유 아이디
-- @old_inven_pos_x			: 이전 아이템 위치 x값
-- @old_inven_pos_y			: 이전 아이템 위치 y값
-- @new_inven_pos_x			: 새로운 아이템 위치 x값
-- @new_inven_pos_y			: 새로운 아이템 위치 y값
-- @new_rotation			: 아이템 회전 r값
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS update_inventory_sp;
GO

CREATE PROCEDURE update_inventory_sp
	@character_id		INT,
	@item_code			INT,
	@old_inven_pos_x	INT,
	@old_inven_pos_y	INT,
	@new_inven_pos_x	INT,
	@new_inven_pos_y	INT,
	@new_rotation		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		UPDATE inventory_tb SET inven_pos_x=@new_inven_pos_x, inven_pos_y=@new_inven_pos_y, rotation=@new_rotation WHERE character_id=@character_id AND item_code=@item_code AND inven_pos_x=@old_inven_pos_x AND inven_pos_y=@old_inven_pos_y
		
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

DECLARE @character_id	INT = 0
DECLARE @item_id		INT = 3
DECLARE @ret			INT

SELECT * FROM inventory_tb

EXEC @ret=update_inventory_sp @character_id, 1, 1, 1 , 1, 1, 0
print @ret

SELECT * FROM inventory_tb

GO