SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Description:	load_server_list(��ū ������Ʈ)
--
-- INPUT Params
-- @server_type		: ���� Ÿ��
-- =============================================
USE shared_database;
DROP PROCEDURE IF EXISTS load_server_list;
GO

CREATE PROCEDURE load_server_list
	@server_type	INT
AS
BEGIN TRY
	BEGIN TRANSACTION
		SET NOCOUNT ON;

		SELECT id, ip, port, name FROM server_tb WHERE type=@server_type

		COMMIT TRANSACTION;
		RETURN 0

END TRY
BEGIN CATCH
	ROLLBACK TRANSACTION;
	RETURN -1
END CATCH
GO

USE shared_database

DECLARE @ret AS INT
EXEC dbo.load_server_list 2


GO