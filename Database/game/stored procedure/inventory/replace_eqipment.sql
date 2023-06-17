SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	replace_eqipment_sp(�κ��丮 ��� ��ü)
--
-- Params
-- @character_id		: ĳ���� ���̵�
-- @item_code			: ������ ���̵�
-- @inven_pos_x			: �κ� ��ġ x��
-- @inven_pos_y			: �κ� ��ġ y��
-- @rotation			: �κ� ȸ�� r��
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS replace_eqipment_sp;
GO

CREATE PROCEDURE replace_eqipment_sp
	@character_id		INT,
	@inven_item_code	INT,
	@inven_pos_x		INT,
	@inven_pos_y		INT,
	@inven_rotation		INT,
	@eqipment_part		INT,
	@eqipment_item_code	INT,
	@eqipment_pos_x		INT,
	@eqipment_pos_y		INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		if(@inven_item_code <> 0)
		BEGIN
			INSERT INTO inventory_tb (character_id, item_code,inven_pos_x, inven_pos_y, rotation) VALUES (@character_id, @inven_item_code, @inven_pos_x, @inven_pos_y, @inven_rotation)
		END

		if(@eqipment_item_code <> 0)
		BEGIN
			DELETE inventory_tb WHERE character_id=@character_id AND item_code=@eqipment_item_code AND inven_pos_x=@eqipment_pos_x AND inven_pos_y=@eqipment_pos_y
		END

		UPDATE dbo.eqipment_tb
		SET
			helmet		= CASE @eqipment_part WHEN 1 THEN @eqipment_item_code ELSE helmet		END,
			shoulders	= CASE @eqipment_part WHEN 2 THEN @eqipment_item_code ELSE shoulders	END,
			chest		= CASE @eqipment_part WHEN 3 THEN @eqipment_item_code ELSE chest		END,
			bracers		= CASE @eqipment_part WHEN 4 THEN @eqipment_item_code ELSE bracers		END,
			hands		= CASE @eqipment_part WHEN 5 THEN @eqipment_item_code ELSE hands		END,
			pants		= CASE @eqipment_part WHEN 6 THEN @eqipment_item_code ELSE pants		END,
			boots		= CASE @eqipment_part WHEN 7 THEN @eqipment_item_code ELSE boots		END,
			weapon_l	= CASE @eqipment_part WHEN 8 THEN @eqipment_item_code ELSE weapon_l		END,
			weapon_r	= CASE @eqipment_part WHEN 9 THEN @eqipment_item_code ELSE weapon_r		END
		WHERE
		character_id=@character_id

		
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

SELECT * FROM eqipment_tb
SELECT * FROM inventory_tb

--������ ������ �߰��ϴ� ���
EXEC dbo.replace_eqipment_sp 1, 0, 0, 0, 0, 1, 31, 2, 2

SELECT * FROM eqipment_tb
SELECT * FROM inventory_tb



--������ ������ ��ü�ϴ� ���
EXEC dbo.replace_eqipment_sp 1, 31, 2, 2, 0, 1, 32, 0, 0

SELECT * FROM eqipment_tb
SELECT * FROM inventory_tb



--������ �ִ��� �����ϴ� ���
EXEC dbo.replace_eqipment_sp 1, 32, 0, 0, 0, 1, 0, 0, 0

SELECT * FROM eqipment_tb
SELECT * FROM inventory_tb


GO