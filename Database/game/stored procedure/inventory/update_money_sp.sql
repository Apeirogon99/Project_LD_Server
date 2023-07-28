SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	update_money_sp(Money Insert & Update)
--
-- Params
-- @character_id		: 캐릭터 아이디
-- @amount				: 수량
-- =============================================
USE game_database;
DROP PROCEDURE IF EXISTS update_money_sp;
GO

CREATE PROCEDURE update_money_sp
	@character_id	INT,
	@amount			INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		--기존에 있었는지 확인
		IF NOT EXISTS (SELECT 1 FROM money_tb WHERE id=@character_id)
			BEGIN
				INSERT INTO money_tb (character_id, amount) VALUES (@character_id, @amount)
			END
		ELSE
			BEGIN
				UPDATE money_tb SET amount=amount+@amount WHERE id=@character_id
			END

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

DECLARE @ret INT
EXEC @ret=update_money_sp 1, 1000

SELECT * FROM money_tb

GO