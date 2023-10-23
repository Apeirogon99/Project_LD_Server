SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	clear_inventory_sp(인벤토리 비우기)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS clear_inventory_sp;
GO

CREATE PROCEDURE clear_inventory_sp
	@character_id	INT

AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DELETE inventory_tb WHERE character_id=@character_id

		UPDATE eqipment_tb SET helmet=0, shoulders=0, chest=0, bracers=0, hands=0, pants=0, boots=0, weapon_l=0, weapon_r=0 WHERE character_id=@character_id
		
		
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

SELECT * FROM inventory_tb WHERE character_id=@character_id
SELECT * FROM eqipment_tb WHERE character_id=@character_id

EXEC @ret=clear_inventory_sp @character_id
print @ret

GO