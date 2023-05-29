SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_inventory_sp(�κ��丮 ����)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @id					: ������ ���� ���̵�
-- @old_inven_pos_x			: ���� ������ ��ġ x��
-- @old_inven_pos_y			: ���� ������ ��ġ y��
-- @new_inven_pos_x			: ���ο� ������ ��ġ x��
-- @new_inven_pos_y			: ���ο� ������ ��ġ y��
-- @new_rotation			: ������ ȸ�� r��
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