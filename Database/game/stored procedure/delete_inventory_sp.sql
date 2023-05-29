SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	delete_inventory_sp(�κ��丮 ����)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @item_code			: ������ �ڵ�
-- @inven_pos_x			: ������ ��ġ x��
-- @inven_pos_y			: ������ ��ġ y��
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS delete_inventory_sp;
GO

CREATE PROCEDURE delete_inventory_sp
	@character_id	INT,
	@item_code		INT,
	@inven_pos_x	INT,
	@inven_pos_y	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		DELETE inventory_tb WHERE character_id=@character_id AND id=@item_code AND inven_pos_x=@inven_pos_x AND inven_pos_y=@inven_pos_y
		
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

SELECT * FROM inventory_tb

DECLARE @character_id	INT = 0
DECLARE @ret			INT

--EXEC @ret=delete_inventory_sp @character_id, 1, 1, 1
print @ret

SELECT * FROM inventory_tb

GO