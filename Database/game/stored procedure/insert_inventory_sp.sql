SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	insert_inventory_sp(�κ��丮 �ε�)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @item_code			: ������ ���̵�
-- @world_pos_x			: ���� ��ġ x��
-- @world_pos_y			: ���� ��ġ y��
-- @world_pos_z			: ���� ��ġ z��
-- @inven_pos_x			: �κ� ��ġ x��
-- @inven_pos_y			: �κ� ��ġ y��
-- @rotation			: �κ� ȸ�� r��
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS insert_inventory_sp;
GO

CREATE PROCEDURE insert_inventory_sp
	@character_id	INT,
	@item_code		INT,
	@world_pos_x	FLOAT,
	@world_pos_y	FLOAT,
	@world_pos_z	FLOAT,
	@inven_pos_x	INT,
	@inven_pos_y	INT,
	@rotation		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		INSERT INTO inventory_tb (character_id, item_code, world_pos_x, world_pos_y, world_pos_z, inven_pos_x, inven_pos_y, rotation) 
		VALUES (@character_id, @item_code, @world_pos_x, @world_pos_y, @world_pos_z, @inven_pos_x, @inven_pos_y, @rotation)
		
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
DECLARE @rotation		INT = 1
DECLARE @ret			INT

DECLARE @NUM INT, @SUM INT,@TIME INT --��������
SET @NUM = 1
SET @SUM = 0
SET @TIME = 100

WHILE @NUM<@TIME
BEGIN 
EXEC @ret=insert_inventory_sp @character_id, @NUM, @NUM, @NUM, @NUM, @NUM, @NUM, @rotation
SET @NUM = @NUM+1
END;

SELECT * FROM inventory_tb

GO