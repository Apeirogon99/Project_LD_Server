SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_inventory_sp(인벤토리 로드)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS load_inventory_sp;
GO

CREATE PROCEDURE load_inventory_sp
	@character_id	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		SELECT item_code, world_pos_x, world_pos_y, world_pos_z, inven_pos_x, inven_pos_y, rotation FROM inventory_tb WHERE character_id=@character_id;
		
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
DECLARE @ret			INT

EXEC @ret=load_inventory_sp @character_id
print @ret

GO